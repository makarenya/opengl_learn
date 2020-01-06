#include "uniform_buffer.h"

TUniformBufferBase::TUniformBufferBase(size_t size) {
    glGenBuffers(1, &Buffer);
    TGlError::Assert("gen uniform buffer");
    try {
        glBindBuffer(GL_UNIFORM_BUFFER, Buffer);
        TGlError::Assert("bind uniform buffer");

        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        TGlError::Assert("set uniform buffer data");

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        TGlError::Assert("unbind uniform buffer");
    } catch (...) {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glDeleteBuffers(1, &Buffer);
        throw;
    }
}

TUniformBufferBase::~TUniformBufferBase() {
    if (Buffer != 0) {
        glDeleteBuffers(1, &Buffer);
    }
}
TUniformBufferBase::TUniformBufferBase(TUniformBufferBase &&src) noexcept
    : Buffer(src.Buffer) {
    src.Buffer = 0;
}
