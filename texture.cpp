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
        case ETextureUsage::Rgb: return SOIL_LOAD_RGB;
        case ETextureUsage::Rgba: return SOIL_LOAD_RGBA;
        default:throw TGlBaseError("invalid value for load image");
    }
}

GLenum ByteFormat(ETextureUsage usage) {
    switch (usage) {
        case ETextureUsage::Rgb:
        case ETextureUsage::Rgba: return GL_UNSIGNED_BYTE;
        case ETextureUsage::Depth: return GL_FLOAT;
        case ETextureUsage::DepthStencil: return GL_UNSIGNED_INT_24_8;
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
        glTexImage2D(what, 0, format, width, height, 0, format, ByteFormat(usage), data);
        TGlError::Assert("set texture image");
    } catch (...) {
        SOIL_free_image_data(data);
    }
    SOIL_free_image_data(data);
}

void DropTexture(GLuint *texture) {
    glDeleteTextures(1, texture);
}

GLuint CreateTexture(const TTextureBuilder& builder) {
    GLuint texture;
    glGenTextures(1, &texture);
    TGlError::Assert("gen texture");
    try {
        glBindTexture(GL_TEXTURE_2D, texture);
        TGlError::Assert("bind texture while create");

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter(builder.MinLinear_, builder.Mipmap_));
        TGlError::Assert("set texture min filter");

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter(builder.MagLinear_));
        TGlError::Assert("set texture mag filter");

        ETextureWrap wrapS = builder.WrapS_ == ETextureWrap::Undefined ? builder.Wrap_ : builder.WrapS_;
        ETextureWrap wrapT = builder.WrapT_ == ETextureWrap::Undefined ? builder.Wrap_ : builder.WrapT_;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrapS));
        TGlError::Assert("set texture wrap s");

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLenum>(wrapT));
        TGlError::Assert("set texture wrap t");

        if (!builder.File_.empty()) {
            LoadTextureImage(builder.File_, GL_TEXTURE_2D, builder.Usage_);
        } else {
            int width, height;
            std::tie(width, height) = builder.Empty_;
            auto format = static_cast<GLenum>(builder.Usage_);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, ByteFormat(builder.Usage_), nullptr);
        }

        TGlError::Assert("set texture image");
        if (builder.Mipmap_ != ETextureMipmap::None) {
            glGenerateMipmap(GL_TEXTURE_2D);
            TGlError::Assert("generate mipmap");
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    } catch (...) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &texture);
        throw;
    }
    return texture;
}


TTexture::TTexture(const TTextureBuilder &builder)
    : Texture(new GLuint(CreateTexture(builder)), DropTexture) {
}

void TTexture::Bind(int location) const {
    if (location < 0) return;
    glActiveTexture(GL_TEXTURE0 + location);
    TGlError::Assert("activate bind texture");
    glBindTexture(GL_TEXTURE_2D, *Texture);
    TGlError::Assert("process bind texture");
}

GLuint CreateCubeTexture(const TCubeTextureBuilder& builder) {
    GLuint texture;
    glGenTextures(1, &texture);
    TGlError::Assert("gen texture");
    try {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        TGlError::Assert("bind texture while create");

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, MinFilter(builder.MinLinear_, builder.Mipmap_));
        TGlError::Assert("set texture min filter");

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, MagFilter(builder.MagLinear_));
        TGlError::Assert("set texture mag filter");

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        TGlError::Assert("set texture wrap s");

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        TGlError::Assert("set texture wrap t");

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        TGlError::Assert("set texture wrap r");

        LoadTextureImage(builder.PosX_, GL_TEXTURE_CUBE_MAP_POSITIVE_X, builder.Usage_);
        LoadTextureImage(builder.NegX_, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, builder.Usage_);
        LoadTextureImage(builder.PosY_, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, builder.Usage_);
        LoadTextureImage(builder.NegY_, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, builder.Usage_);
        LoadTextureImage(builder.PosZ_, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, builder.Usage_);
        LoadTextureImage(builder.NegZ_, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, builder.Usage_);
        if (builder.Mipmap_ != ETextureMipmap::None) {
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            TGlError::Assert("generate mipmap");
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    } catch (...) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glDeleteTextures(1, &texture);
        throw;
    }
    return texture;
}


TCubeTexture::TCubeTexture(const TCubeTextureBuilder &builder)
    : Texture(new GLuint(CreateCubeTexture(builder)), DropTexture) {
}

void TCubeTexture::Bind(int location) const {
    if (location < 0) return;
    glActiveTexture(GL_TEXTURE0 + location);
    TGlError::Assert("activate bind texture");
    glBindTexture(GL_TEXTURE_CUBE_MAP, *Texture);
    TGlError::Assert("process bind texture");
}
