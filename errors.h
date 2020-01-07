#pragma once
#include <string>
#include <GL/glew.h>

class TGlBaseError: public std::exception {
private:
    std::string What;

public:
    explicit TGlBaseError(std::string what)
        : What(std::move(what)) {
    }
    [[nodiscard]] const char *what() const noexcept override {
        return What.c_str();
    }
};

class TGlfwError: public TGlBaseError {
public:
    explicit TGlfwError(const std::string &context)
        : TGlBaseError(ErrorMessage(context)) {
    }

private:
    static std::string ErrorMessage(const std::string &context);
};

class TGlewError: public TGlBaseError {
public:
    TGlewError(GLenum error, const std::string &context)
        : TGlBaseError(ErrorMessage(error, context)) {
    }

private:
    static std::string ErrorMessage(GLenum error, const std::string &context);
};

class TGlError: public TGlBaseError {
public:
    explicit TGlError(const std::string &context)
        : TGlBaseError(ErrorMessage(glGetError(), context)) {
    }

    static void Assert(const std::string &context) {
        int code = glGetError();
        if (code != GL_NO_ERROR) throw TGlError(code, context);
    }

    static void Skip() {
        glGetError();
    }

private:
    explicit TGlError(int code, const std::string &context)
        : TGlBaseError(ErrorMessage(code, context)) {
    }

    static std::string CodeDescription(GLenum code);

    static std::string ErrorMessage(int code, const std::string &context);
};

template<typename T>
T &&ReturnGlAssert(T &&value, std::string file, int line) {
    TGlError::Assert(file + ": " + std::to_string(line));
    return std::forward<T>(value);
}

#define GL_ASSERT(EQ) (EQ); TGlError::Assert(std::string(__FILE__) + ": " + std::to_string(__LINE__))
#define GL_ASSERTR(EQ) ReturnGlAssert((EQ), __FILE__, __LINE__)
