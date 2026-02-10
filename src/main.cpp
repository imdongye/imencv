#include "App.h"

int main() {
    App app("imencv - ImGui Docking", 1280, 720);

    while (!app.should_close()) {
        app.update();
        app.render();
    }

    return 0;
}