#pragma once
#include <vector>
#include <map>
#include <glm/fwd.hpp>


class TShaderProgram {
private:
    GLuint Program;
public:
    TShaderProgram(const std::string &vertexFilename, const std::string &fragmentFilename);
    TShaderProgram(const TShaderProgram &) = delete;
    TShaderProgram &operator=(const TShaderProgram &) = delete;
    ~TShaderProgram();

private:
    static GLuint CreateShader(GLenum type, const std::string &name, const std::string &filename);
    friend class TProgramSetup;
};

class TProgramSetup {
private:
    GLuint Program;
    std::map<std::string, std::tuple<int, GLenum>> Textures;

public:
    explicit TProgramSetup(const TShaderProgram &program);

    TProgramSetup(TProgramSetup &&src) noexcept;
    ~TProgramSetup();

    TProgramSetup(const TProgramSetup &) = delete;
    TProgramSetup &operator=(const TProgramSetup &) = delete;

    TProgramSetup &&Texture(const std::string &name, GLenum type);
    TProgramSetup &&Set(const std::string &name, GLint value);
    TProgramSetup &&Set(const std::string &name, GLfloat value);
    TProgramSetup &&Set(const std::string &name, glm::vec2 value);
    TProgramSetup &&Set(const std::string &name, GLfloat x, GLfloat y);
    TProgramSetup &&Set(const std::string &name, GLfloat x, GLfloat y, GLfloat z);
    TProgramSetup &&Set(const std::string &name, glm::vec3 value);
    TProgramSetup &&Set(const std::string &name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    TProgramSetup &&Set(const std::string &name, glm::vec4 value);
    TProgramSetup &&Set(const std::string &name, const glm::mat4 &mat);
    bool Has(const std::string &name) const;
    int TextureLoc(const std::string &name) const;
    int TryTextureLoc(const std::string &name) const;
    void FlushTextures();

    template<typename ... T>
    TProgramSetup &&TrySet(const std::string &name, T... value) {
        if (Has(name)) {
            Set(name, value...);
        }
        return std::move(*this);
    }

private:
    GLint Location(const std::string &name);
};

