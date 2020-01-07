#include "uniform_buffer.h"

void TUniformBindingBase::Write(const void *data) {
    GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, Buffer));

    void* mapped{};
    try {
        mapped = GL_ASSERTR(glMapBufferRange(GL_UNIFORM_BUFFER, Offset, Size, GL_MAP_WRITE_BIT));
        memcpy(mapped, data, Size);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
    } catch (...) {
        if (mapped != nullptr) {
            glUnmapBuffer(GL_UNIFORM_BUFFER);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        throw;
    }
}

void TUniformBindingBase::Read(void *data) {
    GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, Buffer));
    const void* mapped{};
    try {
        mapped = GL_ASSERTR(glMapBufferRange(GL_UNIFORM_BUFFER, Offset, Size, GL_MAP_READ_BIT));
        memcpy(data, mapped, Size);
        GL_ASSERT(glUnmapBuffer(GL_UNIFORM_BUFFER));
    } catch (...) {
        if (mapped != nullptr) {
            glUnmapBuffer(GL_UNIFORM_BUFFER);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        throw;
    }
}

TUniformBuffer::TUniformBuffer(std::initializer_list<TUniformBindingBase *> buffers) {
    size_t total = 0;
    int index = 0;
    GLint align{};
    GL_ASSERT(glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &align));

    for (auto &buffer : buffers) {
        buffer->Offset = total;
        buffer->BoundIndex = index++;
        total = ((total + buffer->Size - 1) / align + 1) * align;
    }

    GL_ASSERT(glGenBuffers(1, &Buffer));
    try {
        GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, Buffer));

        GL_ASSERT(glBufferData(GL_UNIFORM_BUFFER, total + 2, nullptr, GL_DYNAMIC_DRAW));

        GL_ASSERT(glBindBuffer(GL_UNIFORM_BUFFER, 0));
        for (auto &buffer : buffers) {
            buffer->Buffer = Buffer;
            GL_ASSERT(glBindBufferRange(GL_UNIFORM_BUFFER, buffer->BoundIndex, Buffer, buffer->Offset, buffer->Size));
        }
    } catch (...) {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glDeleteBuffers(1, &Buffer);
        throw;
    }
}

TUniformBuffer::TUniformBuffer(TUniformBuffer &&src) noexcept
    : Buffer(src.Buffer) {
    src.Buffer = 0;
}

TUniformBuffer::~TUniformBuffer() {
    if (Buffer != 0) {
        glDeleteBuffers(1, &Buffer);
        TGlError::Skip();
    }
}
