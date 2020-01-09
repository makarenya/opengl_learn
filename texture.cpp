#include "texture.h"
#include "errors.h"
#include <SOIL/SOIL.h>

GLenum MagFilter(bool magLinear) {
    return magLinear ? GL_LINEAR : GL_NEAREST;
}

GLenum MinFilter(bool minLinear, ETextureMipmap mipmap) {
    switch (mipmap) {
        case ETextureMipmap::None: return minLinear ? GL_LINEAR : GL_NEAREST;
        case ETextureMipmap::Nearest: return minLinear ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
        case ETextureMipmap::Linear: return minLinear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR;
    }
}

int SoilFormat(ETextureUsage usage) {
    switch (usage) {
        case ETextureUsage::SRgb:
        case ETextureUsage::Rgb: return SOIL_LOAD_RGB;
        case ETextureUsage::SRgba:
        case ETextureUsage::Rgba: return SOIL_LOAD_RGBA;
        default:throw TGlBaseError("invalid value for load image");
    }
}

GLenum ByteFormat(ETextureUsage usage) {
    switch (usage) {
        case ETextureUsage::Rgb:
        case ETextureUsage::Rgba:
        case ETextureUsage::SRgb:
        case ETextureUsage::SRgba:
        case ETextureUsage::Depth: return GL_UNSIGNED_BYTE;
        case ETextureUsage::DepthStencil: return GL_UNSIGNED_INT_24_8;
    }
}

GLenum OuterFormat(ETextureUsage usage) {
    switch (usage) {
        case ETextureUsage::SRgb: return GL_RGB;
        case ETextureUsage::SRgba: return GL_RGBA;
        default: return static_cast<GLenum>(usage);
    }
}

void LoadTextureImage(const std::string &file, GLenum what, ETextureUsage usage) {
    int width, height;
    unsigned char *data = SOIL_load_image(file.c_str(), &width, &height, nullptr, SoilFormat(usage));
    if (data == nullptr) {
        throw TGlBaseError("can't load file " + file);
    }
    try {
        auto format = static_cast<GLenum>(usage);
        GL_ASSERT(glTexImage2D(what, 0, format, width, height, 0, OuterFormat(usage), ByteFormat(usage), data));
    } catch (...) {
        SOIL_free_image_data(data);
    }
    SOIL_free_image_data(data);
}

void DropTexture(GLuint *texture) {
    glDeleteTextures(1, texture);
}

GLuint CreateTexture(const TTextureBuilder &builder) {
    GLuint texture;
    GL_ASSERT(glGenTextures(1, &texture));
    try {
        ETextureWrap wrapS = builder.WrapS_ == ETextureWrap::Undefined ? builder.Wrap_ : builder.WrapS_;
        ETextureWrap wrapT = builder.WrapT_ == ETextureWrap::Undefined ? builder.Wrap_ : builder.WrapT_;

        GL_ASSERT(glBindTexture(GL_TEXTURE_2D, texture));
        GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                  MinFilter(builder.MinLinear_, builder.Mipmap_)));
        GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter(builder.MagLinear_)));
        GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrapS)));
        GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLenum>(wrapT)));

        if (!builder.File_.empty()) {
            LoadTextureImage(builder.File_, GL_TEXTURE_2D, builder.Usage_);
        } else {
            int width, height;
            std::tie(width, height) = builder.Empty_;
            auto format = static_cast<GLenum>(builder.Usage_);
            GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                                   ByteFormat(builder.Usage_), nullptr));
        }
        if (builder.Mipmap_ != ETextureMipmap::None) {
            GL_ASSERT(glGenerateMipmap(GL_TEXTURE_2D));
        }
        GL_ASSERT(glBindTexture(GL_TEXTURE_2D, 0));
    } catch (...) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &texture);
        throw;
    }
    return texture;
}

GLuint CreateMultisampleTexture(const TMultiSampleTextureBuilder &builder) {
    GLuint texture;
    GL_ASSERT(glGenTextures(1, &texture));
    try {
        GL_ASSERT(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture));
        auto format = static_cast<GLenum>(builder.Usage_);
        auto[width, height]= builder.Size_;
        GL_ASSERT(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, builder.Samples_, format, width, height, GL_TRUE));
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    } catch (...) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glDeleteTextures(1, &texture);
        throw;
    }
    return texture;
}

GLuint CreateCubeTexture(const TCubeTextureBuilder &builder) {
    GLuint texture;
    GL_ASSERT(glGenTextures(1, &texture));
    try {
        GL_ASSERT(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));
        GL_ASSERT(glTexParameteri(GL_TEXTURE_CUBE_MAP,
                                  GL_TEXTURE_MIN_FILTER,
                                  MinFilter(builder.MinLinear_, builder.Mipmap_)));
        GL_ASSERT(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, MagFilter(builder.MagLinear_)));
        GL_ASSERT(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_ASSERT(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_ASSERT(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

        LoadTextureImage(builder.PosX_, GL_TEXTURE_CUBE_MAP_POSITIVE_X, builder.Usage_);
        LoadTextureImage(builder.NegX_, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, builder.Usage_);
        LoadTextureImage(builder.PosY_, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, builder.Usage_);
        LoadTextureImage(builder.NegY_, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, builder.Usage_);
        LoadTextureImage(builder.PosZ_, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, builder.Usage_);
        LoadTextureImage(builder.NegZ_, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, builder.Usage_);
        if (builder.Mipmap_ != ETextureMipmap::None) {
            GL_ASSERT(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    } catch (...) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glDeleteTextures(1, &texture);
        throw;
    }
    return texture;
}

TTexture::TTexture(const TTextureBuilder &builder)
    : Texture(new GLuint(CreateTexture(builder)), DropTexture)
      , Type(ETextureType::Flat) {
}

TTexture::TTexture(const TMultiSampleTextureBuilder &builder)
    : Texture(new GLuint(CreateMultisampleTexture(builder)), DropTexture)
      , Type(ETextureType::MultiSample) {
}

TTexture::TTexture(const TCubeTextureBuilder &builder)
    : Texture(new GLuint(CreateCubeTexture(builder)), DropTexture)
      , Type(ETextureType::Cube) {
}

int TTexture::GetWidth() const {
    GLint value;
    GL_ASSERT(glBindTexture(static_cast<GLenum>(Type), *Texture));
    GL_ASSERT(glGetTexLevelParameteriv(static_cast<GLenum>(Type), 0, GL_TEXTURE_WIDTH, &value));
    GL_ASSERT(glBindTexture(static_cast<GLenum>(Type), 0));
    return value;
}

int TTexture::GetHeight() const {
    GLint value;
    GL_ASSERT(glBindTexture(static_cast<GLenum>(Type), *Texture));
    GL_ASSERT(glGetTexLevelParameteriv(static_cast<GLenum>(Type), 0, GL_TEXTURE_HEIGHT, &value));
    GL_ASSERT(glBindTexture(static_cast<GLenum>(Type), 0));
    return value;
}

int TTexture::GetDepth() const {
    GLint value;
    GL_ASSERT(glBindTexture(static_cast<GLenum>(Type), *Texture));
    GL_ASSERT(glGetTexLevelParameteriv(static_cast<GLenum>(Type), 0, GL_TEXTURE_DEPTH, &value));
    GL_ASSERT(glBindTexture(static_cast<GLenum>(Type), 0));
    return value;
}

TTextureBinder::~TTextureBinder() {
    bool empty = true;
    for (int i = 0; i < Textures.size(); ++i) {
        if (Textures[i].has_value()) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(static_cast<GLenum>(*Textures[i]), 0);
            empty = false;
        }
    }
    if (!empty)
        glActiveTexture(GL_TEXTURE0);
    TGlError::Skip();
}

TTextureBinder::TTextureBinder(TTextureBinder &&src) noexcept
    : Textures(src.Textures) {
    for (auto &texture : src.Textures) {
        texture = {};
    }
}

void TTextureBinder::Attach(const TTexture &texture, int index) {
    if (texture.Empty()) {
        if (Textures[index].has_value()) {
            GL_ASSERT(glActiveTexture(GL_TEXTURE0 + index));
            GL_ASSERT(glBindTexture(static_cast<GLenum>(*Textures[index]), 0));
            Textures[index] = {};
        }
    } else if (index >= 0) {
        GL_ASSERT(glActiveTexture(GL_TEXTURE0 + index));
        GL_ASSERT(glBindTexture(static_cast<GLenum>(texture.Type), *texture.Texture));
        Textures[index] = texture.Type;
    }
}
