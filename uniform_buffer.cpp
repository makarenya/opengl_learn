#include "uniform_buffer.h"

#define GL_MAP_READ_BIT 0x0001
#define GL_MAP_WRITE_BIT 0x0002

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
    for (auto &buffer : buffers) {
        buffer->Offset = total;
        buffer->BoundIndex = index++;
        total += buffer->Size;
    }
    glGenBuffers(1, &Buffer);
    TGlError::Assert("gen uniform buffer");
    try {
        glBindBuffer(GL_UNIFORM_BUFFER, Buffer);
        TGlError::Assert("bind uniform buffer");

        glBufferData(GL_UNIFORM_BUFFER, total, nullptr, GL_DYNAMIC_DRAW);
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
