#pragma once
#include "errors.h"
#include "shader_program.h"
#include "mesh.h"
#include "texture.h"
#include <GL/glew.h>

class TRenderBuffer {
private:
    std::shared_ptr<GLuint> Buffer;
    ETextureUsage Usage{};
    int Width{};
    int Height{};

public:
    TRenderBuffer() = default;
    TRenderBuffer(ETextureUsage usage, unsigned width, unsigned height, unsigned samples);
    [[nodiscard]] GLuint GetBuffer() const { return *Buffer; }
    [[nodiscard]] int GetWidth() const { return Width; }
    [[nodiscard]] int GetHeight() const { return Height; }
    [[nodiscard]] bool Empty() { return !Buffer; }
};

class TFrameBuffer {
    std::shared_ptr<GLuint> FrameBuffer;
    TTexture ScreenTexture{};
    TRenderBuffer ScreenBuffer{};
    TTexture DepthTexture{};
    TRenderBuffer DepthBuffer{};

public:
    TFrameBuffer(TTexture texture, bool depth = false);
    TFrameBuffer(TTexture screen, TTexture depth);
    TFrameBuffer(TTexture screen, TRenderBuffer depth);
    TFrameBuffer(TRenderBuffer screen, TTexture depth);
    TFrameBuffer(TRenderBuffer screen, TRenderBuffer depth);

    void CopyTo(TFrameBuffer &target);
    [[nodiscard]] TTexture GetScreenTexture() const { return ScreenTexture; }
    [[nodiscard]] TTexture GetDepthTexture() const { return DepthTexture; }

    friend class TFrameBufferBinder;
};

class TFrameBufferBinder {
public:
    explicit TFrameBufferBinder(const TFrameBuffer &framebuffer);
    ~TFrameBufferBinder();

    TFrameBufferBinder(const TFrameBufferBinder &) = delete;
    TFrameBufferBinder &operator=(const TFrameBufferBinder &) = delete;
};
