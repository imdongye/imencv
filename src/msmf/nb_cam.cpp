#include "nb_cam.h"

#include <thread>
#include <atomic>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <turbojpeg.h>
#include <iostream>
#include <algorithm>

// Libraries
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "turbojpeg.lib")

std::vector<std::string> NbCam::get_cam_list() {
    return {}; 
}

std::vector<std::array<int, 3>> NbCam::get_cam_option(int p_id) {
    return {};
}

template <class T>
inline static void mf_release(T*& p) {
    if( p ) {
        p->Release();
        p = nullptr;
    }
}


NbCam::NbCam(): empty_frame(0) {
}
NbCam::~NbCam() {
    close();
}

void NbCam::open() {
    close();

    m2t_kill_thread = false;
    m2t_paused = false;
    t2m_idx_done = -1;
    m_idx_now = -1;

    thread = std::thread(&NbCam::thread_capture_func, this);
}

void NbCam::close() {
    m2t_kill_thread = true;
    if( thread.joinable() ) {
        thread.join();
    }
    t2m_frames.clear();
}

void NbCam::set_paused(bool p_paused) {
    m2t_paused = p_paused;
}

int NbCam::get_state() {
    if( thread.joinable() ) {
        if( m_idx_now<0 ) {
            return 1; // connecting
        }
        return 2; // connected
    }
    return 0; // disconnected
}

const std::vector<uint8_t>& NbCam::get_frame() {
    int latest = t2m_idx_done;

    if( latest<0 ) return empty_frame;
    if( m_idx_now==latest ) return empty_frame;

    m_idx_now = latest;
    return t2m_frames[m_idx_now];
}

void NbCam::thread_capture_func() {
    // start com library
    if( CoInitializeEx(nullptr, COINIT_MULTITHREADED)<0 ) {
        return;
    }
    // msmf engine start
    if( MFStartup(MF_VERSION)<0 ) {
        CoUninitialize();
        return;
    }

    HRESULT hr;
    IMFAttributes* pAttributes = nullptr;
    IMFMediaType* pType = nullptr;
    IMFSourceReader* pReader = nullptr;

    // find devices
    MFCreateAttributes(&pAttributes, 1);
    pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
    IMFActivate** ppDevices = nullptr;
    UINT32 nrDevices;
    hr = MFEnumDeviceSources(pAttributes, &ppDevices, &nrDevices);
    mf_release(pAttributes);
    if( hr<0 || id>=int(nrDevices) ) {
        goto EXIT_POINT;
    }

    // source from device
    IMFMediaSource* pSource = nullptr;
    hr = ppDevices[id]->ActivateObject(IID_PPV_ARGS(&pSource));
    for( UINT32 i=0; i<nrDevices; i++ ) {
        mf_release(ppDevices[i]);
    }
    CoTaskMemFree(ppDevices);
    if( hr<0 ) {
        goto EXIT_POINT;
    }
    
    // create source reader
    // already in worker thread so not use MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING 
    hr = MFCreateSourceReaderFromMediaSource(pSource, nullptr, &pReader);
    mf_release(pSource); 
    if( hr<0 ) {
        goto EXIT_POINT;
    }

    // set options
    MFCreateMediaType(&pType);
    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_MJPG); // !! MJPEG required
    MFSetAttributeSize(pType, MF_MT_FRAME_SIZE, width, height);
    MFSetAttributeRatio(pType, MF_MT_FRAME_RATE, cam_fps, 1);
    hr = pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, pType);
    mf_release(pType);
    if( hr<0 ) {
        goto EXIT_POINT;
    }

    // read frames
    int idxNow = 0;
    int tjRet;
    IMFSample* pSample = nullptr;
    IMFMediaBuffer* pBuffer = nullptr;
    BYTE* pData = nullptr; // MJPEG Raw Data Pointer
    DWORD streamIndex, flags, mjpegLength;
    LONGLONG timestamp;

    // TurboJPEG
    tjhandle tjInstance = tjInitDecompress();

    // reserve ring buffer
    int nrFrames = (cam_fps / std::max(main_fps, 1)) * 2; // safety factor 2.0
    nrFrames = std::max(nrFrames, 10);
    t2m_frames.assign(nrFrames, std::vector<uint8_t>(width * height * 4));


    while( !m2t_kill_thread ) {
        if( m2t_paused ) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        hr = pReader->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0, &streamIndex, &flags, &timestamp, &pSample
        );
        if( hr<0 ) {
            break; // disconnected
        }
        if( pSample==nullptr ) {
            continue;
        }
        if( pSample->ConvertToContiguousBuffer(&pBuffer)>=0 ) {
            if( pBuffer==nullptr ) {
                continue;
            }
            if( pBuffer->Lock(&pData, nullptr, &mjpegLength)>=0 ) {
                if( pData && mjpegLength>0 ) {
                    tjRet = tjDecompress2( // using AVX2
                        tjInstance, 
                        pData, 
                        mjpegLength, 
                        t2m_frames[idxNow].data(),
                        width, 
                        0,
                        height, 
                        TJPF_RGBA, 
                        TJFLAG_FASTDCT | TJFLAG_NOREALLOC // TJFLAG_FASTDCT: fast
                    );
                    if( tjRet==0 ) {
                        t2m_idx_done = idxNow;
                        idxNow = (idxNow + 1) % nrFrames;
                    }
                    pData = nullptr;
                }
                pBuffer->Unlock();
            }
            mf_release(pBuffer);
        }
        mf_release(pSample);
    }

    tjDestroy(tjInstance);

EXIT_POINT:
    mf_release(pReader);
    MFShutdown();
    CoUninitialize();
}

