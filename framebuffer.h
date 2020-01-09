#pragma once
#include "errors.h"
#include "shader_program.h"
#include "mesh.h"
#include "texture.h"
#include <GL/glew.h>

class TFrameBufferBinder {
public:
    explicit TFrameBufferBinder(GLuint frameBuffer);
    ~TFrameBufferBinder();

    TFrameBufferBinder(const TFrameBufferBinder&) = delete;
    TFrameBufferBinder& operator=(const TFrameBufferBinder&) = delete;
};

class IFrameBufferShader {
public:
    virtual ~IFrameBufferShader() = default;
    virtual void SetScreen(const TTexture &texture)= 0;
    virtual void SetDepth(const TTexture &texture) = 0;
};

class TFrameBuffer {
    GLuint FrameBuffer{};
    TTexture Texture;
    std::optional<TTexture> DepthTexture;
    GLuint RenderBuffer{};
    TMesh Mesh;
    unsigned Width;
    unsigned Height;
    unsigned Samples;

public:
    TFrameBuffer(unsigned width, unsigned height, unsigned samples, bool depthAsTexture);
    TFrameBuffer(TFrameBuffer &&src) noexcept;
    TFrameBuffer(const TFrameBuffer &) = delete;
    TFrameBuffer &operator=(const TFrameBuffer &) = delete;
    ~TFrameBuffer();

    TFrameBufferBinder Bind() { return TFrameBufferBinder(FrameBuffer); }

    void Copy(TFrameBuffer &target);
    void Draw(IFrameBufferShader &target);

private:
    void MakeRenderBuffer(int width, int height, int samples);
    void MakeFrameBuffer(bool depthAsTexture);
};

