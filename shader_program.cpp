#include "errors.h"
#include "shader_program.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <fstream>

TProgramSetup::TProgramSetup(const TShaderProgram &program)
    : Program(program.Program), Textures{} {
    GL_ASSERT(glUseProgram(Program));
}

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

TShaderProgram &&TShaderProgram::Block(const std::string &name, const TUniformBindingBase &binding) {
    auto id = GL_ASSERTR(glGetUniformBlockIndex(Program, name.c_str()));
    if (id < 0) throw TGlBaseError("can't find block index for " + name);
    GL_ASSERT(glUniformBlockBinding(Program, id, binding.GetIndex()));
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Texture(const std::string &name, GLenum type) {
    int index = Textures.size();
    glUniform1i(Location(name), index);
    Textures.emplace(name, std::forward_as_tuple(index, type));
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, GLint value) {
    GL_ASSERT(glUniform1i(Location(name), value));
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, GLfloat value) {
    GL_ASSERT(glUniform1f(Location(name), value));
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, glm::vec2 value) {
    GL_ASSERT(glUniform2f(Location(name), value.x, value.y));
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, GLfloat x, GLfloat y) {
    GL_ASSERT(glUniform2f(Location(name), x, y));
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, GLfloat x, GLfloat y, GLfloat z) {
    GL_ASSERT(glUniform3f(Location(name), x, y, z));
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, glm::vec3 value) {
    GL_ASSERT(glUniform3f(Location(name), value.x, value.y, value.z));
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    GL_ASSERT(glUniform4f(Location(name), x, y, z, w));
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, glm::vec4 value) {
    GL_ASSERT(glUniform4f(Location(name), value.x, value.y, value.z, value.w));
    return std::move(*this);
}

TProgramSetup &&TProgramSetup::Set(const std::string &name, const glm::mat4 &mat) {
    GL_ASSERT(glUniformMatrix4fv(Location(name), 1, GL_FALSE, glm::value_ptr(mat)));
    return std::move(*this);
}

bool TProgramSetup::Has(const std::string &name) const {
    auto location = GL_ASSERTR(glGetUniformLocation(Program, name.c_str()));
    return location != -1;
}

int TProgramSetup::TextureLoc(const std::string &name) const {
    try {
        return std::get<0>(Textures.at(name));
    } catch (std::out_of_range &) {
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
    auto location = GL_ASSERTR(glGetUniformLocation(Program, name.c_str()));
    if (location == -1) {
        throw TGlError("can't find location " + name);
    }
    return location;
}

TShaderProgram::TShaderProgram(const std::string &vertexFilename,
                               const std::string &fragmentFilename,
                               const std::string &geometryFilename)
    : Program(glCreateProgram()) {
    if (Program == 0) {
        throw TGlError("create program");
    }
    GLuint vertex{};
    GLuint fragment{};
    GLuint geometry{};
    try {
        vertex = CreateShader(GL_VERTEX_SHADER, "vertex", vertexFilename);
        GL_ASSERT(glAttachShader(Program, vertex));

        fragment = CreateShader(GL_FRAGMENT_SHADER, "fragment", fragmentFilename);
        GL_ASSERT(glAttachShader(Program, fragment));

        if (!geometryFilename.empty()) {
            geometry = CreateShader(GL_GEOMETRY_SHADER, "geometry", geometryFilename);
            GL_ASSERT(glAttachShader(Program, geometry));
        }
        GL_ASSERT(glLinkProgram(Program));
        GLint success;
        glGetProgramiv(Program, GL_LINK_STATUS, &success);
        if (success != GL_TRUE) {
            std::array<char, 512> infoLog{};
            glGetProgramInfoLog(Program, infoLog.size(), nullptr, infoLog.data());
            throw TGlBaseError("link error: " + std::string(infoLog.data()));
        }
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometry != 0) {
            glDeleteShader(geometry);
        }
    } catch (...) {
        if (vertex != 0) glDeleteShader(vertex);
        if (fragment != 0) glDeleteShader(fragment);
        if (geometry != 0) glDeleteShader(geometry);
        glDeleteProgram(Program);
        throw;
    }
}

TShaderProgram::~TShaderProgram() {
    glDeleteProgram(Program);
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
        GL_ASSERT(glShaderSource(shader, 1, &line, nullptr));
        GL_ASSERT(glCompileShader(shader));
        GLint status;
        GL_ASSERT(glGetShaderiv(shader, GL_COMPILE_STATUS, &status));
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
