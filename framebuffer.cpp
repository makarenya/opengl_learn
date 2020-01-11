#include <iostream>
#include "framebuffer.h"
#include "cube.h"

void FreeRenderBuffer(GLuint *buffer) {
    glDeleteRenderbuffers(1, buffer);
}

std::shared_ptr<GLuint> CreateRenderBuffer(ETextureUsage usage, unsigned width, unsigned height, unsigned samples) {
    GLuint buffer;
    auto type = TextureUsageType(usage);
    GL_ASSERT(glGenRenderbuffers(1, &buffer));
    try {
        GL_ASSERT(glBindRenderbuffer(GL_RENDERBUFFER, buffer));
        if (samples == 0) {
            GL_ASSERT(glRenderbufferStorage(GL_RENDERBUFFER, type, width, height));
        } else {
            GL_ASSERT(glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, type, width, height));
        }
        GL_ASSERT(glBindRenderbuffer(GL_RENDERBUFFER, 0));
        return std::shared_ptr<GLuint>(new GLuint(buffer), FreeRenderBuffer);
    } catch (...) {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glDeleteRenderbuffers(1, &buffer);
        throw;
    }
}

TRenderBuffer::TRenderBuffer(ETextureUsage usage, unsigned width, unsigned height, unsigned samples)
    : Buffer(CreateRenderBuffer(usage, width, height, samples))
      , Usage(usage)
      , Width(width)
      , Height(height) {
}

void FreeFrameBuffer(GLuint *framebuffer) {
    glDeleteFramebuffers(1, framebuffer);
}

std::shared_ptr<GLuint> CreateFrameBuffer(TTexture *screenTex,
                                          TTexture *depthTex,
                                          TRenderBuffer *screenBuf,
                                          TRenderBuffer *depthBuf) {
    GLuint framebuffer;
    try {
        GL_ASSERT(glGenFramebuffers(1, &framebuffer));
        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
        if (screenTex != nullptr) {
            auto target = static_cast<GLenum>(screenTex->GetType());
            GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, screenTex->GetTexture(), 0));
        } else if (screenBuf != nullptr) {
            GL_ASSERT(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                                GL_RENDERBUFFER, screenBuf->GetBuffer()));
        } else {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
        if (depthTex != nullptr) {
            auto target = static_cast<GLenum>(depthTex->GetType());
            GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, depthTex->GetTexture(), 0));
        } else if (depthBuf != nullptr) {
            GL_ASSERT(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                                GL_RENDERBUFFER, depthBuf->GetBuffer()));
        }
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw TGlBaseError("Framebuffer Incomplete");
        }
        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        return std::shared_ptr<GLuint>(new GLuint(framebuffer), FreeFrameBuffer);
    } catch (...) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &framebuffer);
        throw;
    }
}

TFrameBuffer::TFrameBuffer(TTexture texture, bool depth)
    : FrameBuffer(CreateFrameBuffer(depth ? nullptr : &texture, depth ? &texture : nullptr, nullptr, nullptr))
      , ScreenTexture(depth ? TTexture{} : texture)
      , DepthTexture(depth ? texture : TTexture{}) {
}

TFrameBuffer::TFrameBuffer(TRenderBuffer buffer, bool depth)
    : FrameBuffer(CreateFrameBuffer(nullptr, nullptr, depth ? nullptr : &buffer, depth ? &buffer : nullptr))
      , ScreenBuffer(depth ? TRenderBuffer{} : buffer)
      , DepthBuffer(depth ? buffer : TRenderBuffer{}) {
}

TFrameBuffer::TFrameBuffer(TTexture screen, TTexture depth)
    : FrameBuffer(CreateFrameBuffer(&screen, &depth, nullptr, nullptr))
      , ScreenTexture(screen)
      , DepthTexture(depth) {
}

TFrameBuffer::TFrameBuffer(TTexture screen, TRenderBuffer depth)
    : FrameBuffer(CreateFrameBuffer(&screen, nullptr, nullptr, &depth))
      , ScreenTexture(screen)
      , DepthBuffer(depth) {
}

TFrameBuffer::TFrameBuffer(TRenderBuffer screen, TTexture depth)
    : FrameBuffer(CreateFrameBuffer(nullptr, &depth, &screen, nullptr))
      , ScreenBuffer(screen)
      , DepthTexture(depth) {
}

TFrameBuffer::TFrameBuffer(TRenderBuffer screen, TRenderBuffer depth)
    : FrameBuffer(CreateFrameBuffer(nullptr, nullptr, &screen, &depth))
      , ScreenBuffer(screen)
      , DepthBuffer(depth) {
}

void TFrameBuffer::CopyTo(TFrameBuffer &target) {
    GLenum copy = 0;
    int srcWidth = 0;
    int srcHeight = 0;
    int dstWidth = 0;
    int dstHeight = 0;
    if (!target.DepthTexture.Empty() && (!DepthTexture.Empty() || !DepthBuffer.Empty())) {
        copy |= GL_DEPTH_BUFFER_BIT;
        srcWidth = !DepthTexture.Empty() ? DepthTexture.GetWidth() : DepthBuffer.GetWidth();
        srcHeight = !DepthTexture.Empty() ? DepthTexture.GetHeight() : DepthBuffer.GetHeight();
        dstWidth = target.DepthTexture.GetWidth();
        dstHeight = target.DepthTexture.GetHeight();
    }
    if (!target.ScreenTexture.Empty() && (!ScreenTexture.Empty() || !ScreenBuffer.Empty())) {
        copy |= GL_COLOR_BUFFER_BIT;
        srcWidth = !ScreenTexture.Empty() ? ScreenTexture.GetWidth() : ScreenBuffer.GetWidth();
        srcHeight = !ScreenTexture.Empty() ? ScreenTexture.GetHeight() : ScreenBuffer.GetHeight();
        dstWidth = target.ScreenTexture.GetWidth();
        dstHeight = target.ScreenTexture.GetHeight();
    }
    GL_ASSERT(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *target.FrameBuffer));
    GL_ASSERT(glBindFramebuffer(GL_READ_FRAMEBUFFER, *FrameBuffer));
    GL_ASSERT(glBlitFramebuffer(0, 0, dstWidth, dstHeight, 0, 0, srcWidth, srcHeight, copy, GL_NEAREST));
    GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

TFrameBufferBinder::TFrameBufferBinder(const TFrameBuffer &framebuffer) {
    GLint current;
    GL_ASSERT(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current));
    GLint viewport[4];
    GL_ASSERT(glGetIntegerv(GL_VIEWPORT, viewport));
    OldBuffer = current;
    OldViewport = glm::ivec4(viewport[0], viewport[1], viewport[2], viewport[3]);

    glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer.FrameBuffer);
    GLenum what = 0;
    int width = 0;
    int height = 0;
    if (!framebuffer.DepthTexture.Empty() || !framebuffer.DepthBuffer.Empty()) {
        what |= GL_DEPTH_BUFFER_BIT;
        width = !framebuffer.DepthTexture.Empty() ? framebuffer.DepthTexture.GetWidth()
                                                  : framebuffer.DepthBuffer.GetWidth();
        height = !framebuffer.DepthTexture.Empty() ? framebuffer.DepthTexture.GetHeight()
                                                  : framebuffer.DepthBuffer.GetHeight();
    }
    if (!framebuffer.ScreenTexture.Empty() || !framebuffer.ScreenBuffer.Empty()) {
        what |= GL_COLOR_BUFFER_BIT;
        width = !framebuffer.ScreenTexture.Empty() ? framebuffer.ScreenTexture.GetWidth()
                                                  : framebuffer.ScreenBuffer.GetWidth();
        height = !framebuffer.ScreenTexture.Empty() ? framebuffer.ScreenTexture.GetHeight()
                                                   : framebuffer.ScreenBuffer.GetHeight();
    }
    GL_ASSERT(glViewport(0, 0, width, height));
    GL_ASSERT(glClearColor(0, 0, 0, 0));
    GL_ASSERT(glClear(what));
}

TFrameBufferBinder::~TFrameBufferBinder() {
    Unbind();
}

void TFrameBufferBinder::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, OldBuffer);
    glViewport(OldViewport[0], OldViewport[1], OldViewport[2], OldViewport[3]);
}
