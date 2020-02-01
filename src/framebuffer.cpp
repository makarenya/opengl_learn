#include "framebuffer.h"
#include "cube.h"

void FreeRenderBuffer(GLuint *buffer) {
    glDeleteRenderbuffers(1, buffer);
}

std::shared_ptr<GLuint> CreateRenderBuffer(ETextureUsage usage, unsigned width, unsigned height, unsigned samples) {
    GLuint buffer;
    auto type = TextureInternalFormat(usage);
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
      , Width(width)
      , Height(height) {
}

void FreeFrameBuffer(GLuint *framebuffer) {
    glDeleteFramebuffers(1, framebuffer);
}

template<typename T>
struct TTargetVisitor {};

template<>
struct TTargetVisitor<const bool &> {
    static void Bind(const bool &arg, GLenum type) {
        if (type == GL_COLOR_ATTACHMENT0) {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
    }
    static glm::ivec2 Size(const bool arg) { return glm::ivec2(0, 0); }
};

template<>
struct TTargetVisitor<const TRenderBuffer &> {
    static void Bind(const TRenderBuffer &arg, GLenum type) {
        GL_ASSERT(glFramebufferRenderbuffer(GL_FRAMEBUFFER, type, GL_RENDERBUFFER, arg.GetBuffer()));
    }
    static glm::ivec2 Size(const TRenderBuffer &arg) { return glm::ivec2(arg.GetWidth(), arg.GetHeight()); }
};

template<ETextureType Type>
struct TTargetVisitor<const TTexture<Type> &> {
    static void Bind(const TTexture<Type> &arg, GLenum type) {
        GL_ASSERT(glFramebufferTexture(GL_FRAMEBUFFER, type, arg.GetTexture(), 0));
    }
    static glm::ivec2 Size(const TTexture<Type> &arg) { return glm::ivec2(arg.GetWidth(), arg.GetHeight()); }
};

std::shared_ptr<GLuint> CreateFrameBuffer(TFrameBufferTarget &screen, TFrameBufferTarget &depth) {
    GLuint framebuffer;
    try {
        GL_ASSERT(glGenFramebuffers(1, &framebuffer));
        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
        std::visit([](const auto &arg) {
            TTargetVisitor<decltype(arg)>::Bind(arg, GL_COLOR_ATTACHMENT0);
        }, screen);
        std::visit([](const auto &arg) {
            TTargetVisitor<decltype(arg)>::Bind(arg, GL_DEPTH_ATTACHMENT);
        }, depth);
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

TFrameBuffer::TFrameBuffer(TFrameBufferTarget screen, TFrameBufferTarget depth)
    : FrameBuffer(CreateFrameBuffer(screen, depth))
      , Screen(screen)
      , Depth(depth) {
}

void TFrameBuffer::CopyTo(TFrameBuffer &target) {
    GLenum copy = 0;
    glm::ivec2 src;
    glm::ivec2 dst;
    if (target.Depth.index() != 0 && Depth.index() != 0) {
        copy |= GL_DEPTH_BUFFER_BIT;
        src = std::visit([](const auto &arg) { return TTargetVisitor<decltype(arg)>::Size(arg); }, Depth);
        dst = std::visit([](const auto &arg) { return TTargetVisitor<decltype(arg)>::Size(arg); }, target.Depth);
    }
    if (target.Screen.index() != 0 && Screen.index() != 0) {
        copy |= GL_COLOR_BUFFER_BIT;
        src = std::visit([](const auto &arg) { return TTargetVisitor<decltype(arg)>::Size(arg); }, Depth);
        dst = std::visit([](const auto &arg) { return TTargetVisitor<decltype(arg)>::Size(arg); }, target.Depth);
    }
    GL_ASSERT(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *target.FrameBuffer));
    GL_ASSERT(glBindFramebuffer(GL_READ_FRAMEBUFFER, *FrameBuffer));
    GL_ASSERT(glBlitFramebuffer(0, 0, dst.x, dst.y, 0, 0, src.x, src.y, copy, GL_NEAREST));
    GL_ASSERT(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    GL_ASSERT(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
    GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

TFrameBufferBinder::TFrameBufferBinder(const TFrameBuffer &framebuffer)
    : Bound(true) {
    GLint current;
    GL_ASSERT(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current));
    GLint viewport[4];
    GL_ASSERT(glGetIntegerv(GL_VIEWPORT, viewport));
    OldBuffer = current;
    OldViewport = glm::ivec4(viewport[0], viewport[1], viewport[2], viewport[3]);

    glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer.FrameBuffer);
    GLenum what = 0;
    glm::ivec2 size;
    if (framebuffer.Depth.index() != 0) {
        what |= GL_DEPTH_BUFFER_BIT;
        size = std::visit([](const auto &arg) { return TTargetVisitor<decltype(arg)>::Size(arg); }, framebuffer.Depth);
    }
    if (framebuffer.Screen.index() != 0) {
        what |= GL_COLOR_BUFFER_BIT;
        size = std::visit([](const auto &arg) { return TTargetVisitor<decltype(arg)>::Size(arg); }, framebuffer.Screen);
    }
    GL_ASSERT(glViewport(0, 0, size.x, size.y));
    GL_ASSERT(glClearColor(0, 0, 0, 0));
    GL_ASSERT(glClear(what));
}

TFrameBufferBinder::~TFrameBufferBinder() {
    Unbind();
}

void TFrameBufferBinder::Unbind() {
    if (Bound) {
        Bound = false;
        glBindFramebuffer(GL_FRAMEBUFFER, OldBuffer);
        glViewport(OldViewport[0], OldViewport[1], OldViewport[2], OldViewport[3]);
    }
}
