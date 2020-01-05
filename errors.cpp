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
        case GL_INVALID_ENUM:return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW:return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY:return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION:return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_CONTEXT_LOST:return "GL_CONTEXT_LOST";
        default:return "Unknown error";
    }
}

std::string TGlError::ErrorMessage(int code, const std::string &context) {
    return "GL " + std::to_string(code) + " : " + CodeDescription(code) + " in " + context;
}
