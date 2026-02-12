#pragma once
#include <vector>
#include <array>
#include <string>
#include <thread>
#include <atomic>

// Non-Blocking Ring Buffer USB Camera
class NbCam {
public:
    static std::vector<std::string> get_cam_list();
    static std::vector<std::array<int, 3>> get_cam_option(int p_id); // array => width, height, fps
public:
    NbCam();
    ~NbCam();

    // read frame hook in thread
    int id, width, height, cam_fps, main_fps = 30;
    void open();
    void set_paused(bool p_paused);
    int get_state(); // 0: disconnected, 1: connecting, 2: connected
    const std::vector<uint8_t>& get_frame();
    void close();
    
private:
    const std::vector<uint8_t> empty_frame;
    std::atomic_bool m2t_paused = true;
    std::atomic_bool m2t_kill_thread = false;
    int m_idx_now = -1;

    void capture_thread_func();
    std::thread thread;
    std::vector<std::vector<uint8_t>> t2m_frames;
    std::atomic_int t2m_idx_done = -1;
};
