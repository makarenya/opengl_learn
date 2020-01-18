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
GLenum TextureUsageType(ETextureUsage usage) {
    switch (usage) {
        case ETextureUsage::Rgb: return GL_RGB;
        case ETextureUsage::Rgba: return GL_RGBA;
        case ETextureUsage::SRgb: return GL_SRGB;
        case ETextureUsage::SRgba: return GL_SRGB_ALPHA;
        case ETextureUsage::Depth: return GL_DEPTH_COMPONENT;
        case ETextureUsage::DepthStencil: return GL_DEPTH24_STENCIL8;
        case ETextureUsage::Height: return GL_DEPTH_COMPONENT;
        case ETextureUsage::Normals: return GL_RGB;
    }
}

int SoilFormat(ETextureUsage usage) {
    switch (usage) {
        case ETextureUsage::SRgb:
        case ETextureUsage::Rgb: return SOIL_LOAD_RGB;
        case ETextureUsage::SRgba:
        case ETextureUsage::Rgba: return SOIL_LOAD_RGBA;
        case ETextureUsage::Depth: return SOIL_LOAD_L;
        case ETextureUsage::Height:
        case ETextureUsage::Normals: return SOIL_LOAD_AUTO;
        default:throw TGlBaseError("invalid value for load image " + std::to_string((int)usage));
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
        case ETextureUsage::Height:
        case ETextureUsage::Normals: return GL_UNSIGNED_BYTE;
    }
}

GLenum OuterFormat(ETextureUsage usage) {
    switch (usage) {
        case ETextureUsage::SRgb: return GL_RGB;
        case ETextureUsage::SRgba: return GL_RGBA;
        default: return TextureUsageType(usage);
    }
}

template<typename T>
T ChannelDeviation(const void *data, int pixels, int channels) {
    const T *scan = static_cast<const T *>(data);
    long deviation = 0;
    for (int i = 0; i < pixels; ++i) {
        int sum = 0;
        int sq = 0;
        for (int c = 0; c < channels; ++c) {
            auto u = *scan++;
            sum += u;
            sq += u * u;
        }
        deviation += (sq * channels - sum * sum) / (channels * channels);
    }
    return static_cast<T>(sqrt(deviation / pixels));
}

std::vector<std::tuple<uint8_t, uint8_t>> Limits(const uint8_t *scan, int pixels, int channels) {
    using namespace std;
    vector<tuple<uint8_t, uint8_t>> limits(channels);
    for (int c = 0; c < channels; ++c) {
        auto p = *scan++;
        limits[c] = make_tuple(p, p);
    }
    for (int i = 1; i < pixels; ++i) {
        for (int c = 0; c < channels; ++c) {
            auto p = *scan++;
            limits[c] = make_tuple(min(get<0>(limits[c]), p), max(get<1>(limits[c]), p));
        }
    }
    return limits;
}

std::vector<uint8_t> HeightMapToNormalMap(const uint8_t *data, int width, int height, int channels, float strength) {
    std::vector<uint8_t> result(width * height * 3);
    uint8_t *p = result.data();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int l = x == 0 ? data[channels * (x + y * width)] : data[channels * (x - 1 + y * width)];
            int r = x == width - 1 ? data[channels * (x + y * width)] : data[channels * (x + 1 + y * width)];
            int t = y == 0 ? data[channels * (x + y * width)] : data[channels * (x + (y - 1) * width)];
            int b = y == height - 1 ? data[channels * (x + y * width)] : data[channels * (x + (y + 1) * width)];
            int dx = l - r;
            int dy = t - b;
            auto length = sqrt(dx * dx + dy * dy + strength * strength);
            *p++ = 127 + static_cast<uint8_t>(127 * dx / length);
            *p++ = 127 + static_cast<uint8_t>(127 * dy / length);
            *p++ = 127 + static_cast<uint8_t>(127 * strength / length);
        }
    }
    return result;
}

std::vector<uint8_t> ReadHeightMap(const uint8_t *data, int width, int height, int channels) {
    std::vector<uint8_t> result(width * height);
    uint8_t *p = result.data();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            *p++ = 255 - *data;
            data += channels;
        }
    }
    return result;
}

std::vector<uint8_t> ReadNormalMap(const uint8_t *data, int width, int height, int channels,
                               unsigned char xm, float xd, unsigned char ym, float yd, unsigned char zm, float zd) {
    std::vector<uint8_t> result(width * height * 3);
    uint8_t *p = result.data();
    for (int i = 0; i < height * width * 3; i += 3) {
        *p++ = 127 + 127 * (data[0] - xm) / xd;
        *p++ = 127 + 127 * (data[1] - ym) / yd;
        *p++ = 127 + 127 * (data[2] - zm) / zd;
        data += channels;
    }
    return result;
}

void LoadTextureImage(const std::string &file, GLenum what, int &width, int &height, ETextureUsage usage) {
    auto format = TextureUsageType(usage);
    if (file.empty()) {
        GL_ASSERT(glTexImage2D(what, 0, format, width, height, 0, format, ByteFormat(usage), nullptr));
    } else {
        int channels;
        unsigned char *const data = SOIL_load_image(file.c_str(), &width, &height, &channels, SoilFormat(usage));
        if (data == nullptr) {
            throw TGlBaseError("can't load file " + file);
        }
        try {
            if (usage == ETextureUsage::Height || usage == ETextureUsage::Normals) {
                std::vector<uint8_t> result;
                if (channels < 3 || ChannelDeviation<unsigned char>(data, width * height, channels) < 2) {
                    // It`s height map.
                    if (usage == ETextureUsage::Height) {
                        result = ReadHeightMap(data, width, height, channels);
                    } else {
                        result = HeightMapToNormalMap(data, width, height, channels, 20.0f);
                    }
                } else {
                    auto limits = Limits(data, width * height, channels);
                    if (std::get<0>(limits[0]) >= 127 && std::get<0>(limits[1]) >= 127
                        && std::get<0>(limits[2]) >= 191) {
                        if (usage == ETextureUsage::Height) {
                            throw TGlBaseError("can't convert normal map to height map");
                        } else {
                            result = ReadNormalMap(data, width, height, channels, 191, 64, 191, 64, 191, 64);
                        }
                    } else {
                        throw TGlBaseError("can't detect format");
                    }
                }
                GL_ASSERT(glTexImage2D(what, 0, format, width, height, 0, OuterFormat(usage),
                                       ByteFormat(usage), result.data()));
            } else {
                GL_ASSERT(glTexImage2D(what, 0, format, width, height, 0, OuterFormat(usage), ByteFormat(usage), data));
            }
        } catch (...) {
            SOIL_free_image_data(data);
            throw;
        }
        SOIL_free_image_data(data);
    }
}

void FreeTexture(std::tuple<GLuint, int, int> *texture) {
    glDeleteTextures(1, &std::get<0>(*texture));
}

void FreeCubeTexture(std::tuple<GLuint, int, int, int> *texture) {
    glDeleteTextures(1, &std::get<0>(*texture));
}

std::shared_ptr<std::tuple<GLuint, int, int>> Impl::CreateFlatTexture(const TTextureBuilder &builder) {
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
        float border[] = {builder.BorderColor_[0], builder.BorderColor_[1],
                          builder.BorderColor_[2], builder.BorderColor_[3]};
        GL_ASSERT(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border));

        auto[width, height] = builder.Empty_;
        LoadTextureImage(builder.File_, GL_TEXTURE_2D, width, height, builder.Usage_);
        if (builder.Mipmap_ != ETextureMipmap::None) {
            GL_ASSERT(glGenerateMipmap(GL_TEXTURE_2D));
        }
        GL_ASSERT(glBindTexture(GL_TEXTURE_2D, 0));
        if (width == 0 || height == 0) {
            throw TGlBaseError("width or height is 0");
        }
        return std::shared_ptr<std::tuple<GLuint, int, int>>(
            new std::tuple<GLuint, int, int>(texture, width, height), FreeTexture);
    } catch (...) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &texture);
        throw;
    }
}

std::shared_ptr<std::tuple<GLuint,
                           int,
                           int>> Impl::CreateMultisampleTexture(const TMultiSampleTextureBuilder &builder) {
    GLuint texture;
    GL_ASSERT(glGenTextures(1, &texture));
    try {
        GL_ASSERT(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture));
        auto format = TextureUsageType(builder.Usage_);
        auto[width, height]= builder.Size_;
        GL_ASSERT(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, builder.Samples_, format, width, height, GL_TRUE));
        GL_ASSERT(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
        if (width == 0 || height == 0) {
            throw TGlBaseError("width or height is 0");
        }
        return std::shared_ptr<std::tuple<GLuint, int, int>>(
            new std::tuple<GLuint, int, int>(texture, width, height), FreeTexture);
    } catch (...) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glDeleteTextures(1, &texture);
        throw;
    }
}

std::shared_ptr<std::tuple<GLuint, int, int, int>> Impl::CreateCubeTexture(const TCubeTextureBuilder &builder) {
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
        auto[width, height, depth] = builder.Empty_;

        LoadTextureImage(builder.PosX_, GL_TEXTURE_CUBE_MAP_POSITIVE_X, depth, height, builder.Usage_);
        LoadTextureImage(builder.NegX_, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, depth, height, builder.Usage_);
        LoadTextureImage(builder.PosY_, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, width, depth, builder.Usage_);
        LoadTextureImage(builder.NegY_, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, width, depth, builder.Usage_);
        LoadTextureImage(builder.PosZ_, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, width, height, builder.Usage_);
        LoadTextureImage(builder.NegZ_, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, width, height, builder.Usage_);
        if (builder.Mipmap_ != ETextureMipmap::None) {
            GL_ASSERT(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
        }
        GL_ASSERT(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
        if (width == 0 || height == 0 || depth == 0) {
            throw TGlBaseError("width, height or depth is 0");
        }
        return std::shared_ptr<std::tuple<GLuint, int, int, int>>(
            new std::tuple<GLuint, int, int, int>(texture, width, height, depth), FreeCubeTexture);
    } catch (...) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glDeleteTextures(1, &texture);
        throw;
    }
}

TTextureBinder::TTextureBinder(TTextureBinder &&src) noexcept
    : Textures(src.Textures) {
    for (auto &texture : src.Textures) {
        texture = {};
    }
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

void TTextureBinder::Attach(ETextureType type, GLuint texture, int index) {
    if (texture == 0) {
        if (Textures[index].has_value()) {
            GL_ASSERT(glActiveTexture(GL_TEXTURE0 + index));
            GL_ASSERT(glBindTexture(static_cast<GLenum>(*Textures[index]), 0));
            Textures[index] = {};
        }
    } else if (index >= 0) {
        GL_ASSERT(glActiveTexture(GL_TEXTURE0 + index));
        GL_ASSERT(glBindTexture(static_cast<GLenum>(type), texture));
        Textures[index] = type;
    }
}
