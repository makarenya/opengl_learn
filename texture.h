#pragma once
#include "common.h"
#include <array>

enum struct ETextureType {
    Flat = GL_TEXTURE_2D,
    MultiSample = GL_TEXTURE_2D_MULTISAMPLE,
    Cube = GL_TEXTURE_CUBE_MAP,
};

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
    SRgb = GL_SRGB,
    SRgba = GL_SRGB_ALPHA,
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

class TMultiSampleTextureBuilder {
public:
    BUILDER_PROPERTY(ETextureUsage, Usage){ETextureUsage::Rgba};
    BUILDER_PROPERTY(int, Samples){4};
    BUILDER_PROPERTY2(int, int, Size);
};

class TTexture {
private:
    std::shared_ptr<GLuint> Texture;
    ETextureType Type;

public:
    TTexture() = default;
    TTexture(const TTextureBuilder &builder);
    TTexture(const TMultiSampleTextureBuilder &builder);
    TTexture(const TCubeTextureBuilder &builder);
    [[nodiscard]] bool Empty() const { return !Texture; }
    GLuint GetTexture() const { return *Texture; }
    ETextureType GetType() const { return Type; }
    int GetWidth() const;
    int GetHeight() const;
    int GetDepth() const;
    friend class TTextureBinder;
};

class TTextureBinder {
private:
    std::array<std::optional<ETextureType>, 32> Textures;

public:
    TTextureBinder() = default;
    ~TTextureBinder();
    TTextureBinder(TTextureBinder &&src) noexcept;
    TTextureBinder(const TTextureBinder &) = delete;
    TTextureBinder &operator=(const TTextureBinder &) = delete;
    void Attach(const TTexture &texture, int index);
};
