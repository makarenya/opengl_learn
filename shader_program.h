#pragma once
#include "binder.h"
#include <vector>
#include <glm/fwd.hpp>

class TProgramSetup {
private:
    GLuint Program;
    std::vector<TBinder> Textures;

public:
    explicit TProgramSetup(GLuint program)
        : Program(program), Textures{} {
    }
    TProgramSetup(TProgramSetup &&src) noexcept;
    ~TProgramSetup();

    TProgramSetup(const TProgramSetup &) = delete;
    TProgramSetup &operator=(const TProgramSetup &) = delete;

    TProgramSetup &&Set(const std::string &name, GLint value);
    TProgramSetup &&Set(const std::string &name, GLfloat value);
    TProgramSetup &&Set(const std::string &name, glm::vec2 value);
    TProgramSetup &&Set(const std::string &name, GLfloat x, GLfloat y);
    TProgramSetup &&Set(const std::string &name, GLfloat x, GLfloat y, GLfloat z);
    TProgramSetup &&Set(const std::string &name, glm::vec3 value);
    TProgramSetup &&Set(const std::string &name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    TProgramSetup &&Set(const std::string &name, glm::vec4 value);
    TProgramSetup &&Set(const std::string &name, const glm::mat4 &mat);
    bool Has(const std::string &name);

private:
    GLint Location(const std::string &name);
};

class TShaderProgram {
private:
    GLuint Program;
public:
    TShaderProgram(const std::string &vertexFilename, const std::string &fragmentFilename);
    TShaderProgram(const TShaderProgram &) = delete;
    TShaderProgram &operator=(const TShaderProgram &) = delete;
    ~TShaderProgram();

    TProgramSetup Use();

private:
    static GLuint CreateShader(GLenum type, const std::string &name, const std::string &filename);
};
