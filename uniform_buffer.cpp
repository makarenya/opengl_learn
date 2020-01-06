#include "uniform_buffer.h"

void TUniformBindingBase::Write(const void *data) {
    glBindBuffer(GL_UNIFORM_BUFFER, Buffer);
    TGlError::Assert("bind buffer while set");

    void* mapped{};
    try {
        mapped = glMapBufferRange(GL_UNIFORM_BUFFER, Offset, Size, GL_MAP_WRITE_BIT);
        TGlError::Assert("map buffer while set");
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
    glBindBuffer(GL_UNIFORM_BUFFER, Buffer);
    TGlError::Assert("bind buffer while get");
    const void* mapped{};
    try {
        mapped = glMapBufferRange(GL_UNIFORM_BUFFER, Offset, Size, GL_MAP_READ_BIT);
        TGlError::Assert("map buffer while get");
        memcpy(data, mapped, Size);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
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
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &align);
    TGlError::Assert("get buffer alignment");

    for (auto &buffer : buffers) {
        buffer->Offset = total;
        buffer->BoundIndex = index++;
        total = ((total + buffer->Size - 1) / align + 1) * align;
    }

    glGenBuffers(1, &Buffer);
    TGlError::Assert("gen uniform buffer");
    try {
        glBindBuffer(GL_UNIFORM_BUFFER, Buffer);
        TGlError::Assert("bind uniform buffer");

        glBufferData(GL_UNIFORM_BUFFER, total + 2, nullptr, GL_DYNAMIC_DRAW);
        TGlError::Assert("set uniform buffer data");

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        TGlError::Assert("unbind uniform buffer");
        for (auto &buffer : buffers) {
            buffer->Buffer = Buffer;
            glBindBufferRange(GL_UNIFORM_BUFFER, buffer->BoundIndex, Buffer, buffer->Offset, buffer->Size);
            TGlError::Assert("bind uniform buffer range");
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
