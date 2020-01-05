#include "errors.h"
#include <GLFW/glfw3.h>

std::string TGlfwError::ErrorMessage(const std::string &context) {
    const char *description;
    const int code = glfwGetError(&description);
    return "GLFW " + std::to_string(code) + " : " + description + " in " + context;
}

std::string TGlewError::ErrorMessage(GLenum error, const std::string &context) {
    return "GLEW " + std::to_string(error) + " : " +
        reinterpret_cast<const char *>(glewGetErrorString(error)) + " in " + context;
}

std::string TGlError::CodeDescription(GLenum code) {
    switch (code) {
        case GL_INVALID_ENUM:return "Given when an enumeration parameter is not a legal enumeration for that function.";
        case GL_INVALID_VALUE:return "Given when a value parameter is not a legal value for that function.";
        case GL_INVALID_OPERATION:return "Given when the set of state for a command is not legal for the parameters given to that command.";
        case GL_STACK_OVERFLOW:return "Given when a stack pushing operation cannot be done because it would overflow the limit of that stack's size.";
        case GL_STACK_UNDERFLOW:return "Given when a stack popping operation cannot be done because the stack is already at its lowest point.";
        case GL_OUT_OF_MEMORY:return "Given when performing an operation that can allocate memory, and the memory cannot be allocated.";
        case GL_INVALID_FRAMEBUFFER_OPERATION:return "Given when doing anything that would attempt to read from or write/render to a framebuffer that is not complete.";
        case GL_CONTEXT_LOST:return "Given if the OpenGL context has been lost, due to a graphics card reset.";
        default:return "Unknown error";
    }
}

std::string TGlError::ErrorMessage(int code, const std::string &context) {
    return "GL " + std::to_string(code) + " : " + CodeDescription(code) + " in " + context;
}
