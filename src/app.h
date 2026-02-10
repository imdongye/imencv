#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class App {
public:
    App(const std::string& p_title, int p_width, int p_height);
    virtual ~App();
    void run();

protected:
    virtual void on_update() = 0;
    virtual void on_render() = 0;
    GLFWwindow* window;

private:
    void init_glfw(const std::string& p_title, int p_width, int p_height);
    void init_imgui();

};