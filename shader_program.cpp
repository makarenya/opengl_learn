#include "errors.h"
#include "shader_program.h"
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <fstream>

TShaderProgram::TShaderProgram(const TShaderBuilder &builder)
    : Program(glCreateProgram()) {
    if (Program == 0) {
        throw TGlError("create program");
    }
    GLuint vertex{};
    GLuint fragment{};
    GLuint geometry{};

    try {
        vertex = CreateShader(GL_VERTEX_SHADER, "vertex", builder.Vertex_);
        GL_ASSERT(glAttachShader(Program, vertex));

        fragment = CreateShader(GL_FRAGMENT_SHADER, "fragment", builder.Fragment_);
        GL_ASSERT(glAttachShader(Program, fragment));

        if (!builder.Geometry_.empty()) {
            geometry = CreateShader(GL_GEOMETRY_SHADER, "geometry", builder.Geometry_);
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
        GL_ASSERT(glDeleteShader(vertex));
        GL_ASSERT(glDeleteShader(fragment));
        if (geometry != 0) {
            GL_ASSERT(glDeleteShader(geometry));
        }

        for (auto &[name, binder] : builder.Blocks_) {
            auto id = GL_ASSERTR(glGetUniformBlockIndex(Program, name.c_str()));
            if (id < 0) throw TGlBaseError("can't find block index for " + name);
            GL_ASSERT(glUniformBlockBinding(Program, id, binder.GetIndex()));
        }

        for (int &texture : Textures)
            texture = -1;
        for (int &color : Colors)
            color = -1;
        for (int &constant : Constants)
            constant = -1;

        for (auto &[name, prop] : builder.Textures_) {
            Textures[static_cast<int>(prop)] = DefineTexture(name, true);
        }
        for (auto &[name, prop] : builder.Colors_) {
            Colors[static_cast<int>(prop)] = DefineProp(name, true);
        }
        for (auto &[name, prop] : builder.Constants_) {
            Constants[static_cast<int>(prop)] = DefineProp(name, true);
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

GLint TShaderProgram::DefineTexture(const std::string &name, bool skip) {
    auto location = DefineProp(name, skip);
    if (location != -1) {
        Bound[TexturesCount] = location;
        return TexturesCount++;
    }
    return -1;
}


GLint TShaderProgram::DefineProp(const std::string &name, bool skip) {
    auto location = GL_ASSERTR(glGetUniformLocation(Program, name.c_str()));
    if (location == -1 && !skip) {
        throw TGlBaseError("can't find property with name " + name);
    }
    return location;
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


TShaderSetup::TShaderSetup(const TShaderProgram *program)
    :  Program(program) {
    GL_ASSERT(glUseProgram(program->Program));
}

TShaderSetup::~TShaderSetup() {
    if (Program != nullptr) {
        glUseProgram(0);
    }
}

void TShaderSetup::Set(GLint location, GLint value) {
    GL_ASSERT(glUniform1i(location, value));
}

void TShaderSetup::Set(GLint location, GLfloat value) {
    GL_ASSERT(glUniform1f(location, value));
}

void TShaderSetup::Set(GLint location, glm::vec2 value) {
    GL_ASSERT(glUniform2f(location, value.x, value.y));
}

void TShaderSetup::Set(GLint location, GLfloat x, GLfloat y) {
    GL_ASSERT(glUniform2f(location, x, y));
}

void TShaderSetup::Set(GLint location, GLfloat x, GLfloat y, GLfloat z) {
    GL_ASSERT(glUniform3f(location, x, y, z));
}

void TShaderSetup::Set(GLint location, glm::vec3 value) {
    GL_ASSERT(glUniform3f(location, value.x, value.y, value.z));
}

void TShaderSetup::Set(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    GL_ASSERT(glUniform4f(location, x, y, z, w));
}

void TShaderSetup::Set(GLint location, glm::vec4 value) {
    GL_ASSERT(glUniform4f(location, value.x, value.y, value.z, value.w));
}

void TShaderSetup::Set(GLint location, const glm::mat4 &mat) {
    GL_ASSERT(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat)));
}

void TShaderSetup::Set(GLint location, GLfloat* data, GLsizei count) {
    GL_ASSERT(glUniform1fv(location, count, data));
}

void TShaderSetup::Set(GLint index, const TMaterialTexture &texture) {
    GL_ASSERT(glUniform1i(Program->Bound[index], index));
    if (texture.index() == 1) {
        Attach(std::get<TFlatTexture>(texture), index);
    } else if (texture.index() == 2) {
        Attach(std::get<TCubeTexture>(texture), index);
    }
}
