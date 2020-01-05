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
        .Vertices(EBufferUsage::Static, Quad<false, true>(
            TGeomBuilder()
                .SetTextureMul(1, -1)
                .SetTextureOffset(0, 1)
                .SetSize(1.0f)))
        .Layout(EDataType::Float, 3)
        .Layout(EDataType::Float, 2))
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

void TFrameBuffer::Draw(TProgramSetup &setup) {
    TTextureBinder binder;
    setup.Set("screenTexture", binder.Attach(Texture));
    if (DepthTexture.has_value()) {
        setup.Set("depthTexture", binder.Attach(*DepthTexture));
    }
    Mesh.Draw();
}

void TFrameBuffer::MakeRenderBuffer(int width, int height) {
    glGenRenderbuffers(1, &RenderBuffer);
    TGlError::Assert("gen renderbuffer");
    try {
        glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer);
        TGlError::Assert("bind renderbuffer");

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        TGlError::Assert("set renderbuffer storage");

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        TGlError::Assert("unbind renderbuffer");
    } catch (...) {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        throw;
    }
}

void TFrameBuffer::MakeFrameBuffer(bool depthAsTexture) {
    try {
        glGenFramebuffers(1, &FrameBuffer);
        TGlError::Assert("gen framebuffer");

        glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
        TGlError::Assert("bind framebuffer");

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture.GetTexture(), 0);
        TGlError::Assert("bind texture to framebuffer");

        if (depthAsTexture) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture->GetTexture(), 0);
            TGlError::Assert("bind texture to framebuffer");
        } else {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBuffer);
            TGlError::Assert("bind renderbuffer to framebuffer");
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw TGlBaseError("Framebuffer Incomplete");
        }
    } catch (...) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        throw;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    TGlError::Assert("unbind framebuffer");
}
