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

TFrameBuffer::TFrameBuffer(int width, int height, bool depthAsTexture)
    : Mesh(
    TMeshBuilder()
        .SetVertices(EBufferUsage::Static, Quad<false, true>(
            TGeomBuilder()
                .SetTextureMul(1, -1)
                .SetTextureOffset(0, 1)
                .SetSize(1.0f)))
        .AddLayout(EDataType::Float, 3)
        .AddLayout(EDataType::Float, 2))
      , Texture(TTextureBuilder()
                    .SetWrap(ETextureWrap::ClampToEdge)
                    .SetEmpty(width, height))
      , DepthTexture(!depthAsTexture ? std::optional<TTexture>() :
                     TTexture(TTextureBuilder()
                                  .SetWrap(ETextureWrap::ClampToEdge)
                                  .SetUsage(ETextureUsage::Depth)
                                  .SetEmpty(width, height))) {
    try {
        if (!depthAsTexture) {
            MakeRenderBuffer(width, height);
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
      , Mesh(std::move(src.Mesh)) {
    src.FrameBuffer = 0;
    src.RenderBuffer = 0;
}

TFrameBuffer::~TFrameBuffer() {
    if (RenderBuffer != 0) {
        glDeleteRenderbuffers(1, &RenderBuffer);
    }
    glDeleteFramebuffers(1, &FrameBuffer);
}

void TFrameBuffer::Draw(IFrameBufferShader &target) {
    target.SetScreen(Texture);
    if (DepthTexture.has_value()) {
        target.SetDepth(*DepthTexture);
    }
    Mesh.Draw();
}

void TFrameBuffer::MakeRenderBuffer(int width, int height) {
    GL_ASSERT(glGenRenderbuffers(1, &RenderBuffer));
    try {
        GL_ASSERT(glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer));
        GL_ASSERT(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
        GL_ASSERT(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    } catch (...) {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        throw;
    }
}

void TFrameBuffer::MakeFrameBuffer(bool depthAsTexture) {
    try {
        GL_ASSERT(glGenFramebuffers(1, &FrameBuffer));
        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer));
        GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture.GetTexture(), 0));

        if (depthAsTexture) {
            GL_ASSERT(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture->GetTexture(), 0));
        } else {
            GL_ASSERT(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBuffer));
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
