#include "framebuffer.h"
#include "cube.h"

TFrameBufferBinder::TFrameBufferBinder(GLuint frameBuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

TFrameBufferBinder::~TFrameBufferBinder() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TFrameBuffer::TFrameBuffer(unsigned width, unsigned height, unsigned samples, bool depthAsTexture)
    : Mesh(
    TMeshBuilder()
        .SetVertices(EBufferUsage::Static, Quad<false, true>(
            TGeomBuilder()
                .SetTextureMul(1, -1)
                .SetTextureOffset(0, 1)
                .SetSize(1.0f)))
        .AddLayout(EDataType::Float, 3)
        .AddLayout(EDataType::Float, 2))
      , Texture(samples == 0 ? TTexture{TTextureBuilder()
                                            .SetWrap(ETextureWrap::ClampToEdge)
                                            .SetEmpty(width, height)}
                             : TTexture{TMultiSampleTextureBuilder()
                                            .SetSamples(samples)
                                            .SetSize(width, height)})
      , DepthTexture(!depthAsTexture ? std::optional<TTexture>()
                                     : samples == 0 ? TTexture{TTextureBuilder()
                                                                   .SetWrap(ETextureWrap::ClampToEdge)
                                                                   .SetUsage(ETextureUsage::Depth)
                                                                   .SetEmpty(width, height)}
                                                    : TTexture{TMultiSampleTextureBuilder()
                                                                   .SetSamples(samples)
                                                                   .SetUsage(ETextureUsage::Depth)
                                                                   .SetSize(width, height)})
      , Width(width)
      , Height(height)
      , Samples(samples) {
    try {
        if (!depthAsTexture) {
            MakeRenderBuffer(width, height, samples);
        }
        MakeFrameBuffer(depthAsTexture);
    } catch (...) {
        if (RenderBuffer != 0) {
            glDeleteRenderbuffers(1, &RenderBuffer);
        }
        if (FrameBuffer != 0) {
            glDeleteFramebuffers(1, &FrameBuffer);
        }
        throw;
    }
}

TFrameBuffer::TFrameBuffer(TFrameBuffer &&src) noexcept
    : FrameBuffer(src.FrameBuffer)
      , Texture(std::move(src.Texture))
      , DepthTexture(std::move(src.DepthTexture))
      , RenderBuffer(src.RenderBuffer)
      , Mesh(std::move(src.Mesh))
      , Samples(src.Samples) {
    src.FrameBuffer = 0;
    src.RenderBuffer = 0;
}

TFrameBuffer::~TFrameBuffer() {
    if (RenderBuffer != 0) {
        glDeleteRenderbuffers(1, &RenderBuffer);
    }
    glDeleteFramebuffers(1, &FrameBuffer);
}

void TFrameBuffer::Copy(TFrameBuffer &target) {
    GL_ASSERT(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.FrameBuffer));
    GL_ASSERT(glBindFramebuffer(GL_READ_FRAMEBUFFER, FrameBuffer));
    GL_ASSERT(glBlitFramebuffer(0, 0, target.Width, target.Height, 0, 0, Width, Height,
                                GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST));
    GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void TFrameBuffer::Draw(IFrameBufferShader &target) {
    target.SetScreen(Texture);
    if (DepthTexture.has_value()) {
        target.SetDepth(*DepthTexture);
    }
    Mesh.Draw();
}

void TFrameBuffer::MakeRenderBuffer(int width, int height, int samples) {
    GL_ASSERT(glGenRenderbuffers(1, &RenderBuffer));
    try {
        GL_ASSERT(glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer));
        if (samples == 1) {
            GL_ASSERT(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
        } else {
            GL_ASSERT(glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height));
        }
        GL_ASSERT(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    } catch (...) {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        throw;
    }
}

void TFrameBuffer::MakeFrameBuffer(bool depthAsTexture) {
    auto type = static_cast<GLenum>(Texture.GetType());
    try {
        GL_ASSERT(glGenFramebuffers(1, &FrameBuffer));
        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer));
        GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, type, Texture.GetTexture(), 0));

        if (depthAsTexture) {
            GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, type, DepthTexture->GetTexture(), 0));
        } else {
            GL_ASSERT(glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                                GL_DEPTH_STENCIL_ATTACHMENT,
                                                GL_RENDERBUFFER,
                                                RenderBuffer));
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw TGlBaseError("Framebuffer Incomplete");
        }
    } catch (...) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        throw;
    }
    GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
