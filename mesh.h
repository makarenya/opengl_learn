#pragma once

#include <GL/glew.h>
#include <map>
#include <string>
#include <vector>
#include "shader_program.h"

enum struct EBufferUsage {
    Static,
    Dynamic,
    Stream
};

enum struct EDataType {
    Byte,
    UByte,
    Short,
    UShort,
    Int,
    UInt,
    Float,
    Double
};

enum struct EDrawType {
    Triangles = GL_TRIANGLES,
    Points = GL_POINTS,
};

class TMesh;

class TMeshBuilder {
private:
    GLuint Vbo{};
    GLuint Ebo{};
    GLuint InstanceVbo{};
    std::vector<std::tuple<GLenum, unsigned, unsigned, unsigned>> Locations{};
    unsigned Stride{};
    unsigned InstanceStride{};
    unsigned VertexCount{};
    unsigned IndexCount{};
    unsigned InstanceCount{};

public:
    TMeshBuilder() = default;
    ~TMeshBuilder();

    TMeshBuilder(const TMeshBuilder &) = delete;
    TMeshBuilder &operator=(const TMeshBuilder &) = delete;

    TMeshBuilder &&Vertices(EBufferUsage usage, const void *data, unsigned length, unsigned count);
    TMeshBuilder &&Indices(EBufferUsage usage, const void *data, unsigned length, unsigned count);
    TMeshBuilder &&Instances(EBufferUsage usage, const void *data, unsigned length, unsigned count);
    TMeshBuilder &&Layout(EDataType type, unsigned count, unsigned divisor = 0);

    template<typename T>
    TMeshBuilder &&Vertices(EBufferUsage usage, const T &src) {
        return Vertices(usage, src.data(), sizeof(typename T::value_type) * src.size(), src.size());
    }

    template<typename T, int N>
    TMeshBuilder &&Vertices(EBufferUsage usage, const T src[N]) {
        return Vertices(usage, src, sizeof(T) * N);
    }

    template<typename T>
    TMeshBuilder &&Indices(EBufferUsage usage, const T &src) {
        return Indices(usage, src.data(), sizeof(typename T::value_type) * src.size(), src.size());
    }

    template<typename T, int N>
    TMeshBuilder &&Indices(EBufferUsage usage, const T src[N]) {
        return Indices(usage, src, sizeof(T) * N);
    }

private:
    static GLenum OpenGlAccess(EBufferUsage usage);
    static GLenum OpenGlDataType(EDataType type);
    static GLenum DataSize(EDataType type);
    GLuint BuildVao();
    friend class TMesh;
};


class TMesh {
private:
    GLuint Vao;
    GLuint Vbo;
    GLuint Ebo;
    GLuint InstanceVbo;
    unsigned VertexCount;
    unsigned IndexCount;
    unsigned InstanceCount;

public:
    TMesh(TMeshBuilder &&builder);
    TMesh(TMesh &&src) noexcept;
    ~TMesh();
    TMesh(const TMesh &) = delete;
    TMesh &operator=(const TMesh &) = delete;

    void Draw(EDrawType type = EDrawType::Triangles) const;
    friend class TVertexBufferMapper;
    friend class TInstanceBufferMapper;
};

class TVertexBufferMapper {
    void *Data;
public:
    explicit TVertexBufferMapper(TMesh &mesh, bool instances);
    TVertexBufferMapper(TVertexBufferMapper &&src) noexcept;
    ~TVertexBufferMapper();
    TVertexBufferMapper(const TVertexBufferMapper &) = delete;
    TVertexBufferMapper &operator=(const TVertexBufferMapper &) = delete;

    template<typename T>
    T *Ptr() {
        return static_cast<T *>(Data);
    }
};
