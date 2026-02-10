#include "enet_app.h"
#include "network_utils.h"


EnetApp::EnetApp() : App("ENet Chat App", 1280, 720) {
    if (enet_initialize() != 0) {
        fprintf(stderr, "Error: Failed to initialize ENet\n");
    }
}
EnetApp::~EnetApp() {
    if (host) enet_host_destroy(host);
    enet_deinitialize();
}

void EnetApp::on_update() {
    process_network_events();

    // 연결 전 화면 (IDLE, SERVER 대기 중, CLIENT 연결 시도 중)
    if (status != AppStatus::CONNECTED) {
        ImGui::Begin("네트워크 설정");

        if (status == AppStatus::IDLE) {
            ImGui::InputInt("포트", &port);
            if (ImGui::Button("서버 시작 (대기 모드)")) {
                local_ips = get_local_ips();
                start_server(port);
            }
            ImGui::Separator();
            ImGui::InputText("서버 IP", ip_buffer, sizeof(ip_buffer));
            if (ImGui::Button("서버 연결 (참가 모드)")) {
                start_client(ip_buffer, port);
            }
        } 
        else if (status == AppStatus::SERVER) {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "서버 활성화됨! 클라이언트를 기다리는 중...");
            ImGui::Text("포트: %d", port);
            ImGui::Text("아래 IP 중 하나를 클라이언트에 입력하세요:");
            for (const auto& ip : local_ips) {
                ImGui::BulletText("%s", ip.c_str());
            }
            if (ImGui::Button("중단")) { 
                if(host) { enet_host_destroy(host); host = nullptr; }
                status = AppStatus::IDLE; 
            }
        } 
        else if (status == AppStatus::CLIENT) {
            ImGui::Text("서버(%s:%d)에 연결 시도 중...", ip_buffer, port);
            if (ImGui::Button("취소")) { status = AppStatus::IDLE; }
        }

        ImGui::End();
    } 
    // 연결 성공 후 채팅 화면
    else {
        ImGui::Begin("채팅창");
        // ... (이전 채팅 UI 코드와 동일) ...
        ImGui::Text("상대방과 연결되었습니다.");
        
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
        for (const auto& log : chat_log) {
            ImGui::TextUnformatted(log.c_str());
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();

        if (ImGui::InputText("메시지", chat_buffer, sizeof(chat_buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            send_chat(chat_buffer);
            chat_buffer[0] = '\0';
            ImGui::SetKeyboardFocusHere(-1); // 입력 후 포커스 유지
        }
        ImGui::SameLine();
        if (ImGui::Button("전송")) {
            send_chat(chat_buffer);
            chat_buffer[0] = '\0';
        }
        ImGui::End();
    }
}

void EnetApp::start_server(int p_port) {
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = p_port;

    host = enet_host_create(&address, 32, 2, 0, 0);
    if (host) {
        status = AppStatus::SERVER;
        chat_log.push_back("[시스템] 서버가 열렸습니다.");
    }
}

void EnetApp::start_client(const std::string& p_ip, int p_port) {
    host = enet_host_create(NULL, 1, 2, 0, 0);
    if (!host) return;

    ENetAddress address;
    enet_address_set_host(&address, p_ip.c_str());
    address.port = p_port;

    peer = enet_host_connect(host, &address, 2, 0);
    status = AppStatus::CLIENT;
}

void EnetApp::process_network_events() {
    if (!host) return;

    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                status = AppStatus::CONNECTED;
                peer = event.peer;
                chat_log.push_back("[시스템] 새로운 연결이 감지되었습니다.");
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                chat_log.push_back((char*)event.packet->data);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                status = AppStatus::IDLE;
                chat_log.push_back("[시스템] 연결이 끊어졌습니다.");
                break;
        }
    }
}

void EnetApp::send_chat(const std::string& p_msg) {
    if (p_msg.empty() || !peer) return;

    std::string full_msg = (status == AppStatus::SERVER) ? "Server: " : "Client: ";
    full_msg += p_msg;

    ENetPacket* packet = enet_packet_create(full_msg.c_str(), full_msg.length() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    
    chat_log.push_back(full_msg); // 내 화면에도 표시
}

void EnetApp::on_render() {
    glClearColor(0.1f, 0.13f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}