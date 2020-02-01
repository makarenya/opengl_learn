#pragma once
#include "common.h"

enum EBufferType {
    Array = GL_ARRAY_BUFFER,
    Index = GL_ELEMENT_ARRAY_BUFFER
};

enum struct EBufferUsage {
    Static = GL_STATIC_DRAW,
    Dynamic = GL_DYNAMIC_DRAW,
    Stream = GL_STREAM_DRAW
};

namespace Impl {
    std::shared_ptr<GLuint> Create(GLenum type, EBufferUsage usage, const void *data, size_t size);
    void Change(GLenum type, GLuint buffer, EBufferUsage usage, const void *data, size_t size);
    void Write(GLenum type, GLuint buffer, const void *data, size_t offset, size_t size);
    void *MapBuffer(GLenum type, GLuint buffer, bool write = true);
    void *MapBuffer(GLenum type, GLuint buffer, size_t offset, size_t size, bool write = true);
    void UnmapBuffer(GLenum type);
    void BindBuffer(GLenum type, GLuint buffer);
    void UnBindBuffer(GLenum type);
}

template<EBufferType Type>
class TBuffer {
private:
    std::shared_ptr<GLuint> Buffer;
    size_t Size = 0;

public:
    TBuffer() = default;
    TBuffer(EBufferUsage usage, const void *data, size_t size)
        : Buffer(Impl::Create(static_cast<GLenum>(Type), usage, data, size))
          , Size(size) {
    }

    template<typename T>
    TBuffer(EBufferUsage usage, T &&src)
        : Buffer(Impl::Create(static_cast<GLenum>(Type),
                              usage,
                              src.data(),
                              sizeof(typename std::remove_reference<T>::type::value_type) * src.size()))
          , Size(sizeof(T) * src.size()) {
    }

    template<typename T, size_t N>
    TBuffer(EBufferUsage usage, T src[N])
        : Buffer(Create(static_cast<GLenum>(Type), usage, src, N))
          , Size(sizeof(T) * N) {
    }

    void Change(EBufferUsage usage, const void *data, size_t size) {
        Impl::Change(Type, *Buffer, usage, data, size);
    }

    template<typename T>
    void Change(EBufferUsage usage, T &&src) {
        Impl::Change(Type, *Buffer, usage, src.data(), sizeof(typename T::value_type) * src.size());
    }

    template<typename T, size_t N>
    void Change(EBufferUsage usage, T src[N]) {
        Impl::Change(Type, *Buffer, usage, src, sizeof(T) * N);
    }

    void Write(void *data, int offset, int length) {
        Impl::Write(Type, *Buffer, data, offset, length);
    }

    [[nodiscard]] bool Empty() const {
        return !Buffer;
    }

private:
    template<EBufferType, typename T>
    friend
    class TBufferMapper;
    template<EBufferType>
    friend
    class TBufferBinder;
};

template<EBufferType Type, typename T>
class TBufferMapper {
    void *Data;

public:
    explicit TBufferMapper(const TBuffer<Type> &buffer, bool write = true)
        : Data(Impl::MapBuffer(static_cast<GLenum>(Type), *buffer.Buffer, write)) {
    }

    explicit TBufferMapper(const TBuffer<Type> &buffer, size_t offset, size_t size, bool write = true)
        : Data(Impl::MapBuffer(static_cast<GLenum>(Type), *buffer.Buffer, offset, size, write)) {
    }

    ~TBufferMapper() {
        Impl::UnmapBuffer(static_cast<GLenum>(Type));
    }

    TBufferMapper(const TBufferMapper &) = delete;
    TBufferMapper &operator=(const TBufferMapper &) = delete;

    T* Ptr() {
        return static_cast<T*>(Data);
    }

    T *operator*() {
        return static_cast<T *>(Data);
    }
};

template<EBufferType Type>
class TBufferBinder {
private:
    bool Bound;

public:
    explicit TBufferBinder(const TBuffer<Type> &buffer)
        : Bound(!buffer.Empty()) {
        if (Bound) {
            Impl::BindBuffer(static_cast<GLenum>(Type), *buffer.Buffer);
        }
    }
    ~TBufferBinder() {
        if (Bound) {
            Impl::UnBindBuffer(static_cast<GLenum>(Type));
        }
    }
    TBufferBinder(const TBufferBinder &) = delete;
    TBufferBinder &operator=(const TBufferBinder &) = delete;
};

using TArrayBuffer = TBuffer<EBufferType::Array>;
using TIndexBuffer = TBuffer<EBufferType::Index>;
template<typename T>
using TArrayBufferMapper = TBufferMapper<EBufferType::Array, T>;
template<typename T>
using TIndexBufferMapper = TBufferMapper<EBufferType::Index, T>;
using TArrayBinder = TBufferBinder<EBufferType::Array>;
using TIndexBinder = TBufferBinder<EBufferType::Index>;
