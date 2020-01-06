#pragma once
#include "common.h"
#include "shader_program.h"

class TUniformBufferBase {
public:
    GLuint Buffer{};
    int BoundIndex{-1};
    explicit TUniformBufferBase(size_t size);
    TUniformBufferBase(TUniformBufferBase &&src) noexcept;
    ~TUniformBufferBase();

    void Bind(TShaderProgram& program, const std::string& name) {
        auto id = glGetUniformBlockIndex(program.GetProgram(), name.c_str());
        TGlError::Assert("get block index");
        glUniformBlockBinding(program.GetProgram(), id, BoundIndex);
        TGlError::Assert("bind shader");
    }

    TUniformBufferBase(const TUniformBufferBase &) = delete;
    TUniformBufferBase &operator=(const TUniformBufferBase &) = delete;
};

template<typename T>
class TUniformBuffer: public TUniformBufferBase {
public:
    TUniformBuffer() : TUniformBufferBase(sizeof(T)) {
    }

    TUniformBuffer &operator=(T value) {
        glBindBuffer(GL_UNIFORM_BUFFER, Buffer);
        TGlError::Assert("bind buffer while set");
        try {
            auto *data = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
            TGlError::Assert("map buffer while set");
            memcpy(data, static_cast<void *>(&value), sizeof(value));
            glUnmapBuffer(GL_UNIFORM_BUFFER);
        } catch (...) {
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            throw;
        }
        return *this;
    }

    operator T() {
        glBindBuffer(GL_UNIFORM_BUFFER, Buffer);
        TGlError::Assert("bind buffer while get");
        try {
            auto *data = glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_ONLY);
            TGlError::Assert("map buffer while get");
            T result = *reinterpret_cast<T *>(data);
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            return result;
        } catch (...) {
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            throw;
        }
    }
};

class TUniformConnector {
private:
    std::vector<TUniformBufferBase *> Buffers;

public:
    TUniformConnector(std::vector<TUniformBufferBase *> buffers) : Buffers(std::move(buffers)) {
        int i = 0;
        for (auto buffer : Buffers) {
            glBindBufferBase(GL_UNIFORM_BUFFER, i, buffer->Buffer);
            buffer->BoundIndex = i;
            TGlError::Assert("bind buffer base");
            i++;
        }
    }
};



