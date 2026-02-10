#pragma once
#include "app.h"
#include <imgui.h>

class EnetApp : public App {
public:
    EnetApp() : App("ENet Test App", 1280, 720) {}

protected:
    void on_update() override {
        ImGui::DockSpaceOverViewport();
        ImGui::Begin("네트워크 설정");
        ImGui::Text("ENet 서버 상태: 대기 중...");
        if (ImGui::Button("서버 시작")) {
        }
        ImGui::End();
    }

    void on_render() override {
        glClearColor(0.1f, 0.13f, 0.16f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
};