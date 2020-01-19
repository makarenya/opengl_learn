#pragma once
#include "common.h"
#include "uniform_buffer.h"
#include "texture.h"
#include "material.h"

class TShaderBuilder {
public:
    BUILDER_PROPERTY2(const unsigned char *, size_t, Vertex);
    BUILDER_PROPERTY2(const unsigned char *, size_t, Fragment);
    BUILDER_PROPERTY2(const unsigned char *, size_t, Geometry);
    BUILDER_MAP2(EMaterialProp, std::string, std::string, Texture){};
    BUILDER_MAP(EMaterialProp, std::string, Color){};
    BUILDER_MAP(EMaterialProp, std::string, Constant){};
    BUILDER_MAP(std::string, TUniformBindingBase, Block){};
    TShaderBuilder &&SetTexture(EMaterialProp p, const std::string &v) {
        Textures_.emplace(p, std::forward_as_tuple(v, ""));
        return std::move(*this);
    }
};

class TShaderProgram {
private:
    GLuint Program;
    std::array<GLint, EMaterialProp::MATERIAL_PROPS_COUNT> Textures{};
    std::array<GLint, EMaterialProp::MATERIAL_PROPS_COUNT> TextureSwitches{};
    std::array<GLint, EMaterialProp::MATERIAL_PROPS_COUNT> Colors{};
    std::array<GLint, EMaterialProp::MATERIAL_PROPS_COUNT> Constants{};
    std::array<GLint, 32> Bound{};
    int TexturesCount = 1;

public:
    TShaderProgram(const TShaderBuilder &builder);
    TShaderProgram(const TShaderProgram &) = delete;
    TShaderProgram &operator=(const TShaderProgram &) = delete;
    ~TShaderProgram();

protected:
    GLint DefineTexture(const std::string &name, bool skip = false);
    GLint DefineProp(const std::string &name, bool skip = false);

private:
    static GLuint CreateShader(GLenum type, const std::string &name, const unsigned char *body, size_t length);
    friend class TShaderSetup;
};

class TShaderSetup: public TTextureBinder, public IMaterialBound {
private:
    const TShaderProgram *Program;

public:
    explicit TShaderSetup(const TShaderProgram *program);
    TShaderSetup(TShaderSetup &&src) noexcept
        : TTextureBinder(std::move(src))
          , Program(src.Program) {
        src.Program = nullptr;
    }
    ~TShaderSetup() override;

    void SetTexture(EMaterialProp prop, const TMaterialTexture &texture) override {
        auto index = Program->Textures[static_cast<size_t>(prop)];
        if (index != -1) {
            Set(index, texture);
        }
        auto sw = Program->TextureSwitches[static_cast<size_t>(prop)];
        if (sw != -1) {
            Set(sw, texture.index() > 0);
        }
    }

    void SetColor(EMaterialProp prop, glm::vec4 color) override {
        auto location = Program->Colors[static_cast<size_t>(prop)];
        if (location != -1) {
            Set(location, color);
        }
    }

    void SetConstant(EMaterialProp prop, float value) override {
        auto location = Program->Constants[static_cast<size_t>(prop)];
        if (location != -1) {
            Set(location, value);
        }
    }

protected:
    static void Set(GLint location, GLint value);
    static void Set(GLint location, GLfloat value);
    static void Set(GLint location, glm::vec2 value);
    static void Set(GLint location, GLfloat x, GLfloat y);
    static void Set(GLint location, GLfloat x, GLfloat y, GLfloat z);
    static void Set(GLint location, glm::vec3 value);
    static void Set(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    static void Set(GLint location, glm::vec4 value);
    static void Set(GLint location, const GLfloat* data, GLsizei count);
    static void Set(GLint location, const glm::mat4 &mat);
    static void Set(GLint location, const glm::mat3 &mat);
    static void Set(GLint location, const glm::mat4 *mat, GLsizei count);
    void Set(GLint index, const TMaterialTexture &texture);
};
