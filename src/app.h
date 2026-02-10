#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class App {
public:
    App(const std::string& p_title, int p_width, int p_height);
    ~App();

    bool should_close() const;
    void update();
    void render();

private:
    void init_glfw(const std::string& p_title, int p_width, int p_height);
    void init_imgui();

    GLFWwindow* window;
    bool show_demo_window = true;
    float bg_color[3] = { 0.1f, 0.1f, 0.1f };
};