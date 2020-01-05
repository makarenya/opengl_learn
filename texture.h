#pragma once
#include "common.h"
#include <array>


enum struct ETextureMipmap {
    None,
    Nearest,
    Linear
};

enum struct ETextureWrap {
    Undefined = 0,
    Repeat = GL_REPEAT,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER
};

enum struct ETextureUsage {
    Rgb = GL_RGB,
    Rgba = GL_RGBA,
    Depth = GL_DEPTH_COMPONENT,
    DepthStencil = GL_DEPTH24_STENCIL8
};

class TTextureBuilder {
public:
    BUILDER_PROPERTY(bool, MagLinear){true};
    BUILDER_PROPERTY(bool, MinLinear){true};
    BUILDER_PROPERTY(ETextureMipmap, Mipmap){ETextureMipmap::None};
    BUILDER_PROPERTY(ETextureWrap, Wrap){ETextureWrap::Repeat};
    BUILDER_PROPERTY(ETextureWrap, WrapS){ETextureWrap::Undefined};
    BUILDER_PROPERTY(ETextureWrap, WrapT){ETextureWrap::Undefined};
    BUILDER_PROPERTY(ETextureUsage, Usage){ETextureUsage::Rgba};
    BUILDER_PROPERTY(std::string, File);
    BUILDER_PROPERTY(glm::vec4, BorderColor);
    BUILDER_PROPERTY2(unsigned, unsigned, Empty){0, 0};

    BUILDER_PROPERTY(std::string, PosX);
    BUILDER_PROPERTY(std::string, NegX);
    BUILDER_PROPERTY(std::string, PosY);
    BUILDER_PROPERTY(std::string, NegY);
    BUILDER_PROPERTY(std::string, PosZ);
    BUILDER_PROPERTY(std::string, NegZ);
};

class TTexture {
private:
    std::shared_ptr<GLuint> Texture;
public:
    TTexture(const TTextureBuilder &builder);
    GLuint GetTexture() const { return *Texture; }
    void Bind(int location) const ;
};

class TCubeTextureBuilder {
public:
    BUILDER_PROPERTY(bool, MagLinear){true};
    BUILDER_PROPERTY(bool, MinLinear){true};
    BUILDER_PROPERTY(ETextureMipmap, Mipmap){ETextureMipmap::None};
    BUILDER_PROPERTY(ETextureUsage, Usage){ETextureUsage::Rgba};
    BUILDER_PROPERTY(std::string, PosX);
    BUILDER_PROPERTY(std::string, NegX);
    BUILDER_PROPERTY(std::string, PosY);
    BUILDER_PROPERTY(std::string, NegY);
    BUILDER_PROPERTY(std::string, PosZ);
    BUILDER_PROPERTY(std::string, NegZ);
};

class TCubeTexture {
private:
    std::shared_ptr<GLuint> Texture;
public:
    TCubeTexture(const TCubeTextureBuilder &builder);
    void Bind(int location) const;
};
