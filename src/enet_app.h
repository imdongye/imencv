#pragma once
#include "app.h"
#include <imgui.h>
#include <enet/enet.h>
#include <vector>
#include <string>

enum class AppStatus {
    IDLE,
    SERVER,
    CLIENT,
    CONNECTED
};

class EnetApp : public App {
public:
    EnetApp() : App("ENet Chat App", 1280, 720) {
        if (enet_initialize() != 0) {
            fprintf(stderr, "Error: Failed to initialize ENet\n");
        }
    }

    ~EnetApp() {
        if (host) enet_host_destroy(host);
        enet_deinitialize();
    }

protected:
    void on_update() override;
    void on_render() override;

private:
    void start_server(int p_port);
    void fetch_local_ips();
    void start_client(const std::string& p_ip, int p_port);
    void process_network_events();
    void send_chat(const std::string& p_msg);

    // Network members
    AppStatus status = AppStatus::IDLE;
    ENetHost* host = nullptr;
    ENetPeer* peer = nullptr; // For client: server connection, For server: last connected client

    // UI state
    int port = 12345;
    char ip_buffer[128] = "127.0.0.1";
    char chat_buffer[256] = "";
    std::vector<std::string> chat_log;

    std::vector<std::string> local_ips;
};