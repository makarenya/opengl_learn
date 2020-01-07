#include "buffer.h"

namespace {
    void FreeBuffer(GLuint *buf) {
        glDeleteBuffers(1, buf);
    }
}

namespace Impl {
    std::shared_ptr<GLuint> Create(GLenum type, EBufferUsage usage, const void *data, size_t size) {
        GLuint buffer;
        GL_ASSERT(glGenBuffers(1, &buffer));
        try {
            GL_ASSERT(glBindBuffer(type, buffer));
            GL_ASSERT(glBufferData(type, size, data, static_cast<GLenum>(usage)));
        } catch (...) {
            glBindBuffer(type, 0);
            glDeleteBuffers(1, &buffer);
            throw;
        }
        GL_ASSERT(glBindBuffer(type, 0));
        return std::shared_ptr<GLuint>(new GLuint(buffer), FreeBuffer);
    }

    void Change(GLenum type, GLuint buffer, EBufferUsage usage, const void *data, size_t size) {
        GL_ASSERT(glBindBuffer(type, buffer));
        try {
            GL_ASSERT(glBufferData(type, size, data, static_cast<GLenum>(usage)));
        } catch (...) {
            glBindBuffer(type, 0);
            throw;
        }
        GL_ASSERT(glBindBuffer(type, 0));
    }

    void *MapBuffer(GLenum type, GLuint buffer, bool write) {
        GL_ASSERT(glBindBuffer(type, buffer));
        return GL_ASSERTR(glMapBuffer(type, write ? GL_WRITE_ONLY : GL_READ_ONLY));
    }

    void *MapBuffer(GLenum type, GLuint buffer, size_t offset, size_t size, bool write) {
        GL_ASSERT(glBindBuffer(type, buffer));
        return GL_ASSERTR(glMapBufferRange(type, offset, size, write ? GL_MAP_WRITE_BIT : GL_MAP_READ_BIT));
    }

    void UnmapBuffer(GLenum type) {
        glUnmapBuffer(type);
        TGlError::Skip();
    }

    void BindBuffer(GLenum type, GLuint buffer) {
        GL_ASSERT(glBindBuffer(type, buffer));
    }

    void UnBindBuffer(GLenum type) {
        glBindBuffer(type, 0);
    }
}

