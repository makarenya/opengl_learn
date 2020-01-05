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

class TFrameBuffer {
    GLuint FrameBuffer{};
    TTexture Texture;
    std::optional<TTexture> DepthTexture;
    GLuint RenderBuffer{};
    TMesh Mesh;

public:
    TFrameBuffer(int width, int height, bool depthAsTexture);
    TFrameBuffer(TFrameBuffer &&src) noexcept;
    TFrameBuffer(const TFrameBuffer &) = delete;
    TFrameBuffer &operator=(const TFrameBuffer &) = delete;
    ~TFrameBuffer();

    TFrameBufferBinder Bind() { return TFrameBufferBinder(FrameBuffer); }

    void Draw(TProgramSetup& setup);

private:
    void MakeRenderBuffer(int width, int height);
    void MakeFrameBuffer(bool depthAsTexture);
};

