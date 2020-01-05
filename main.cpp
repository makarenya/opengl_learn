#include "errors.h"
#include "scene.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iomanip>
#include <unistd.h>

using namespace std;
using namespace glm;

bool Keys[1024];
vec2 Mouse{NAN, NAN};

void KeyboardCallback(GLFWwindow *, int key, int, int action, int) {
    if (action == GLFW_PRESS) {
        Keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        Keys[key] = false;
    }
}

void MouseMoveCallback(GLFWwindow *, double xpos, double ypos) {
    Mouse = vec2(xpos, ypos);
}

void PrintMat(glm::mat4 mat) {
    auto w = std::setw(7);
    cout << setfill(' ') << std::setprecision(2) << fixed
         << w << mat[0][0] << ", " << w << mat[1][0] << ", " << w << mat[2][0] << ", " << w << mat[3][0] << endl
         << w << mat[0][1] << ", " << w << mat[1][1] << ", " << w << mat[2][1] << ", " << w << mat[3][1] << endl
         << w << mat[0][2] << ", " << w << mat[1][2] << ", " << w << mat[2][2] << ", " << w << mat[3][2] << endl
         << w << mat[0][3] << ", " << w << mat[1][3] << ", " << w << mat[2][3] << ", " << w << mat[3][3] << endl
         << endl;
}

double gauss(double x, double mu, double sigma) {
    return 1.0 / (sigma * std::sqrt(M_2_PI)) * std::exp(-(x - mu) * (x - mu) / (2 * sigma * sigma));
}

void kernel() {
    double sigma = 1.3;
    double flat = 0.1;
    int size = 3;

    double top = gauss(0, 0, sigma);

    double total = 0.0;
    for (int y = 0; y < 2 * size + 1; y++) {
        for (int x = 0; x < 2 * size + 1; x++) {
            double length = std::sqrt((y - size) * (y - size) + (x - size) * (x - size));
            total += length < flat ? top : gauss(length, flat, sigma);
        }
    }

    for (int y = 0; y < 2 * size + 1; y++) {
        for (int x = 0; x < 2 * size + 1; x++) {
            double length = std::sqrt((y - size) * (y - size) + (x - size) * (x - size));
            double value = length < flat ? top : gauss(length, flat, sigma);
            cout << fixed << setprecision(4) << value / total << ", ";
        }
        cout << endl;
    }
}

void spot() {
    float radius = 1.2;
    int size = static_cast<int>(std::ceil(radius - 0.5));
    float r2 = radius * radius;
    for (int y = 0; y < 2 * size + 1; y++) {
        for (int x = 0; x < 2 * size + 1; x++) {
            int hits = 0;
            for (int i = 0; i < 11; i++) {
                for (int j = 0; j < 11; j++) {
                    float py = (y * 11 + j) - size * 11 - 5;
                    float px = (x * 11 + i) - size * 11 - 5;
                    double length = (px * px + py * py) / 121;
                    if (length < r2) hits++;
                }
            }
            cout << fixed << setprecision(4) << hits / 121.0 << ", ";
        }
        cout << endl;
    }
}

void program() {
    kernel();

    if (glfwInit() != GLFW_TRUE) {
        throw TGlfwError("init");
    }

    try {
        int monitorsCount = 0;
        auto monitors = glfwGetMonitors(&monitorsCount);
        auto mode = glfwGetVideoMode(monitors[1]);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        auto window = glfwCreateWindow(mode->width, mode->height, "OpenGL example", monitors[1], nullptr);
        if (window == nullptr) {
            throw TGlfwError("create window");
        }
        glfwMakeContextCurrent(window);

        glewExperimental = GL_TRUE;
        GLenum initResult = glewInit();
        if (initResult != GLEW_OK) {
            throw TGlewError(initResult, "init");
        }

        glViewport(0, 0, mode->width, mode->height);
        TGlError::Assert("gl viewport");

        glEnable(GL_DEPTH_TEST);
        TGlError::Assert("enable depth test");
        glEnable(GL_CULL_FACE);
        TGlError::Assert("enable cull face test");
        glEnable(GL_BLEND);
        TGlError::Assert("enable blend");
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        TGlError::Assert("enable blend func");

        const float sensibility = .25f;
        const float speed = 30.0f;
        double lastTime = glfwGetTime();
        vec3 position{0.0f, 14.0f, 22.0f};
        vec2 lastMouse = Mouse;
        vec3 up{0.0f, 1.0f, 0.0f};
        float pitch = 0;
        float yaw = -90;

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetKeyCallback(window, KeyboardCallback);
        glfwSetCursorPosCallback(window, MouseMoveCallback);

        TScene scene(mode->width, mode->height);
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            TGlError::Assert("poll events");

            if (!std::isnan(lastMouse.x) && !std::isnan(lastMouse.y)) {
                auto rot = Mouse - lastMouse;
                yaw += rot.x * sensibility;
                pitch -= rot.y * sensibility;
                pitch = std::max(std::min(pitch, 80.0f), -80.0f);
            }
            lastMouse = Mouse;
            vec3 direction = glm::normalize(vec3{cos(glm::radians(pitch)) * cos(glm::radians(yaw)),
                                                 sin(glm::radians(pitch)),
                                                 cos(glm::radians(pitch)) * sin(glm::radians(yaw))});

            auto time = glfwGetTime();
            auto interval = static_cast<float>(time - lastTime);
            lastTime = time;
            cout << static_cast<int>(1.0 / interval) << endl;
            usleep(20000);

            if (Keys[GLFW_KEY_ESCAPE]) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            vec3 flatDir = normalize(vec3(direction.x, 0.0f, direction.z));
            vec3 movement = vec3(0, 0, 0);
            if (Keys[GLFW_KEY_W] || Keys[GLFW_KEY_UP]) {
                movement += flatDir;
            }
            if (Keys[GLFW_KEY_S] || Keys[GLFW_KEY_DOWN]) {
                movement -= flatDir;
            }
            if (Keys[GLFW_KEY_D] || Keys[GLFW_KEY_RIGHT]) {
                movement += cross(flatDir, up);
            }
            if (Keys[GLFW_KEY_A] || Keys[GLFW_KEY_LEFT]) {
                movement -= cross(flatDir, up);
            }
            if (movement != vec3(0, 0, 0)) {
                position += normalize(movement) * interval * speed;
            }

            mat4 view = lookAt(vec3(position), vec3(position + direction), up);
            mat4 project = perspective(radians(45.0f), 1.0f * mode->width / mode->height, 0.1f, 300.0f);
            scene.Draw(project, view, position);

            glfwSwapBuffers(window);
            TGlError::Assert("swap buffers");
        }
    } catch (TGlBaseError &) {
        glfwTerminate();
        throw;
    }
}

int main() {
    try {
        program();
    } catch (TGlBaseError &e) {
        cout << e.what() << endl;
        return 1;
    } catch (...) {
        return 2;
    }
    glfwTerminate();
    return 0;
}
