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
    Rgb,
    Rgba,
    SRgb,
    SRgba,
    Depth,
    FloatDepth,
    DepthStencil,
};

GLenum TextureUsageType(ETextureUsage usage);

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
    BUILDER_PROPERTY2(int, int, Empty){0, 0};
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
    BUILDER_PROPERTY3(int, int, int, Empty){0, 0, 0};
};

class TMultiSampleTextureBuilder {
public:
    BUILDER_PROPERTY(ETextureUsage, Usage){ETextureUsage::Rgba};
    BUILDER_PROPERTY(int, Samples){4};
    BUILDER_PROPERTY2(int, int, Size);
};

namespace Impl {
    std::shared_ptr<std::tuple<GLuint, int, int>> CreateFlatTexture(const TTextureBuilder &builder);
    std::shared_ptr<std::tuple<GLuint, int, int>> CreateMultisampleTexture(const TMultiSampleTextureBuilder &builder);
    std::shared_ptr<std::tuple<GLuint, int, int, int>> CreateCubeTexture(const TCubeTextureBuilder &builder);
}

template<ETextureType>
class TTexture;

template<>
class TTexture<ETextureType::Flat> {
private:
    std::shared_ptr<std::tuple<GLuint, int, int>> Texture;

public:
    TTexture(const TTextureBuilder &builder)
        : Texture(Impl::CreateFlatTexture(builder)) {
    }
    GLuint GetTexture() const { return std::get<0>(*Texture); }
    int GetWidth() const { return std::get<1>(*Texture); }
    int GetHeight() const { return std::get<2>(*Texture); }
    friend class TTextureBinder;
};

template<>
class TTexture<ETextureType::MultiSample> {
private:
    std::shared_ptr<std::tuple<GLuint, int, int>> Texture;

public:
    TTexture(const TMultiSampleTextureBuilder &builder)
        : Texture(Impl::CreateMultisampleTexture(builder)) {
    }
    GLuint GetTexture() const { return std::get<0>(*Texture); }
    int GetWidth() const { return std::get<1>(*Texture); }
    int GetHeight() const { return std::get<2>(*Texture); }
    friend class TTextureBinder;
};

template<>
class TTexture<ETextureType::Cube> {
private:
    std::shared_ptr<std::tuple<GLuint, int, int, int>> Texture;

public:
    TTexture(const TCubeTextureBuilder &builder)
        : Texture(Impl::CreateCubeTexture(builder)) {
    }
    GLuint GetTexture() const { return std::get<0>(*Texture); }
    int GetWidth() const { return std::get<1>(*Texture); }
    int GetHeight() const { return std::get<2>(*Texture); }
    int GetDepth() const { return std::get<3>(*Texture); }
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
    template<ETextureType Type>
    void Attach(const TTexture<Type> &texture, int index) {
        Attach(Type, texture.GetTexture(), index);
    }

private:
    void Attach(ETextureType type, GLuint texture, int index);
};

using TFlatTexture = TTexture<ETextureType::Flat>;
using TCubeTexture = TTexture<ETextureType::Cube>;
using TMultiSampleTexture = TTexture<ETextureType::MultiSample>;
