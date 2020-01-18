#pragma once
#include "errors.h"
#include "shader_program.h"
#include "mesh.h"
#include "texture.h"
#include <GL/glew.h>

class TRenderBuffer {
private:
    std::shared_ptr<GLuint> Buffer;
    int Width{};
    int Height{};

public:
    TRenderBuffer() = default;
    TRenderBuffer(ETextureUsage usage, unsigned width, unsigned height, unsigned samples);
    [[nodiscard]] GLuint GetBuffer() const { return *Buffer; }
    [[nodiscard]] int GetWidth() const { return Width; }
    [[nodiscard]] int GetHeight() const { return Height; }
    [[nodiscard]] bool Empty() const { return !Buffer; }
};

using TFrameBufferTarget = std::variant<bool,
                                        TRenderBuffer,
                                        TFlatTexture,
                                        TMultiSampleTexture,
                                        TCubeTexture>;

class TFrameBuffer {
    std::shared_ptr<GLuint> FrameBuffer;
    TFrameBufferTarget Screen;
    TFrameBufferTarget Depth;

public:
    TFrameBuffer(TFrameBufferTarget screen, TFrameBufferTarget depth);

    void CopyTo(TFrameBuffer &target);
    [[nodiscard]] TFrameBufferTarget GetScreen() const { return Screen; }
    [[nodiscard]] TFrameBufferTarget GetDepth() const { return Depth; }
    friend class TFrameBufferBinder;
};

class TFrameBufferBinder {
    bool Bound;
    GLuint OldBuffer{};
    glm::ivec4 OldViewport{};
public:
    explicit TFrameBufferBinder(const TFrameBuffer &framebuffer);
    ~TFrameBufferBinder();

    TFrameBufferBinder(const TFrameBufferBinder &) = delete;
    TFrameBufferBinder &operator=(const TFrameBufferBinder &) = delete;
    void Unbind();
};
