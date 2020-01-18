#pragma once
#include "common.h"

class TUniformBindingBase {
private:
    GLuint Buffer{};
    size_t Size;
    size_t Offset{};
    GLuint BoundIndex{};

public:
    explicit TUniformBindingBase(size_t size) : Size(size) {
    }
    [[nodiscard]] GLuint GetIndex() const { return BoundIndex; }
    void Write(const void* data);

    friend class TUniformBuffer;
};

template<typename T>
class TUniformBinding: public TUniformBindingBase {
public:
    TUniformBinding() : TUniformBindingBase(sizeof(T)) {
    }

    TUniformBinding &operator=(const T& value) {
        Write(&value);
        return *this;
    }

    explicit operator T() {
        T result{};
        Read(result);
        return result;
    }
};

class TUniformBuffer {
private:
    GLuint Buffer{};

public:
    TUniformBuffer(std::initializer_list<TUniformBindingBase *> buffers);
    TUniformBuffer(TUniformBuffer&& src) noexcept;
    ~TUniformBuffer();

    TUniformBuffer(const TUniformBuffer&) = delete;
    TUniformBuffer& operator=(const TUniformBuffer&) = delete;
};

