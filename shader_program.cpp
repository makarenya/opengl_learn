#include "errors.h"
#include "shader_program.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <fstream>

TProgramSetup::TProgramSetup(TProgramSetup &&src) noexcept {
    Program = src.Program;
    Textures = std::move(src.Textures);
    src.Program = 0;
}

TProgramSetup::~TProgramSetup() {
    FlushTextures();
    if (Program != 0) {
        glUseProgram(0);
    }
}

TProgramSetup &&TProgramSetup::Texture(const std::string& name, GLenum type) {
    int index = Textures.size();
    glUniform1i(Location(name), index);
    Textures.emplace(name, std::forward_as_tuple(index, type));
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, GLint value) {
    glUniform1i(Location(name), value);
    TGlError::Assert("set value " + name);
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, GLfloat value) {
    glUniform1f(Location(name), value);
    TGlError::Assert("set value " + name);
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, glm::vec2 value) {
    glUniform2f(Location(name), value.x, value.y);
    TGlError::Assert("set value " + name);
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, GLfloat x, GLfloat y) {
    glUniform2f(Location(name), x, y);
    TGlError::Assert("set value " + name);
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, GLfloat x, GLfloat y, GLfloat z) {
    glUniform3f(Location(name), x, y, z);
    TGlError::Assert("set value " + name);
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, glm::vec3 value) {
    glUniform3f(Location(name), value.x, value.y, value.z);
    TGlError::Assert("set value " + name);
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    glUniform4f(Location(name), x, y, z, w);
    TGlError::Assert("set value " + name);
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, glm::vec4 value) {
    glUniform4f(Location(name), value.x, value.y, value.z, value.w);
    TGlError::Assert("set value " + name);
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, const glm::mat4 &mat) {
    glUniformMatrix4fv(Location(name), 1, GL_FALSE, glm::value_ptr(mat));
    TGlError::Assert("set value " + name);
    return std::move(*this);
}

bool TProgramSetup::Has(const std::string &name) const {
    auto location = glGetUniformLocation(Program, name.c_str());
    TGlError::Assert("uniform location " + name);
    return location != -1;
}

int TProgramSetup::TextureLoc(const std::string &name) const {
    try {
        return std::get<0>(Textures.at(name));
    } catch (std::out_of_range&) {
        throw TGlBaseError(name + " not found");
    }
}

int TProgramSetup::TryTextureLoc(const std::string &name) const {
    auto index = Textures.find(name);
    if (index != Textures.end()) {
        return std::get<0>(index->second);
    }
    return -1;
}

void TProgramSetup::FlushTextures() {
    for (auto tex : Textures) {
        glActiveTexture(GL_TEXTURE0 + std::get<0>(tex.second));
        glBindTexture(std::get<1>(tex.second), 0);
    }
    glActiveTexture(GL_TEXTURE0);
}

GLint TProgramSetup::Location(const std::string &name) {
    auto location = glGetUniformLocation(Program, name.c_str());
    TGlError::Assert("uniform location " + name);
    if (location == -1) {
        throw TGlError("can't find location " + name);
    }
    return location;
}

TShaderProgram::TShaderProgram(const std::string& vertexFilename, const std::string& fragmentFilename)
    : Program(glCreateProgram()) {
    if (Program == 0) {
        throw TGlError("create program");
    }
    GLuint vertex{};
    GLuint fragment{};
    try {
        vertex = CreateShader(GL_VERTEX_SHADER, "vertex", vertexFilename);
        fragment = CreateShader(GL_FRAGMENT_SHADER, "fragment", fragmentFilename);
        glAttachShader(Program, vertex);
        TGlError::Assert("attach vertex shader");
        glAttachShader(Program, fragment);
        TGlError::Assert("attach fragment shader");
        glLinkProgram(Program);
        TGlError::Assert("link program");
        GLint success;
        glGetProgramiv(Program, GL_LINK_STATUS, &success);
        if (success != GL_TRUE) {
            std::array<char, 512> infoLog{};
            glGetProgramInfoLog(Program, infoLog.size(), nullptr, infoLog.data());
            throw TGlBaseError("link error: " + std::string(infoLog.data()));
        }
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    } catch (...) {
        if (vertex != 0) glDeleteShader(vertex);
        if (fragment != 0) glDeleteShader(fragment);
        glDeleteProgram(Program);
        throw;
    }
}

TShaderProgram::~TShaderProgram() {
    glDeleteProgram(Program);
}

TProgramSetup TShaderProgram::Use() {
    glUseProgram(Program);
    TGlError::Assert("use program");
    return TProgramSetup(Program);
}

GLuint TShaderProgram::CreateShader(GLenum type, const std::string &name, const std::string &filename) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        throw TGlError("vertex shader");
    }
    try {
        std::string code;
        try {
            std::ifstream fileStream{filename};
            code = {std::istreambuf_iterator<char>{fileStream}, std::istreambuf_iterator<char>{}};
        } catch (std::istream::failure &e) {
            throw TGlBaseError("error loading " + name + " while " + e.what());
        }
        const char *line = code.c_str();
        glShaderSource(shader, 1, &line, nullptr);
        TGlError::Assert(name + " shader source");
        glCompileShader(shader);
        TGlError::Assert(name + " compile shader");
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        TGlError::Assert("compile " + name + " shader");
        if (status != GL_TRUE) {
            std::array<char, 512> infoLog{};
            glGetShaderInfoLog(shader, infoLog.size(), nullptr, infoLog.data());
            throw TGlBaseError(name + " compilation error: " + infoLog.data());
        }
        return shader;
    } catch (...) {
        glDeleteShader(shader);
        throw;
    }
}
