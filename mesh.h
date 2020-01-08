#pragma once
#include "common.h"
#include "buffer.h"

enum struct EDataType {
    Byte = GL_BYTE,
    UByte = GL_UNSIGNED_BYTE,
    Short = GL_SHORT,
    UShort = GL_UNSIGNED_SHORT,
    Int = GL_INT,
    UInt = GL_UNSIGNED_INT,
    Float = GL_FLOAT,
    Double = GL_DOUBLE
};

enum struct EDrawType {
    Triangles = GL_TRIANGLES,
    Points = GL_POINTS,
};

class TMeshBuilder {
public:
    BUILDER_PROPERTY2(TArrayBuffer, unsigned, Vertices);
    BUILDER_PROPERTY2(TIndexBuffer, unsigned, Indices);
    BUILDER_PROPERTY2(TArrayBuffer, unsigned, Instances);
    BUILDER_LIST3(EDataType, unsigned, unsigned, Layout);

    template<typename T>
    TMeshBuilder &SetVertices(EBufferUsage usage, T &&src) {
        SetVertices({usage, std::forward<T>(src)}, src.size());
        return *this;
    }

    template<typename T>
    TMeshBuilder &SetIndices(EBufferUsage usage, T &&src) {
        SetIndices({usage, std::forward<T>(src)}, src.size());
        return *this;
    }

    template<typename T>
    TMeshBuilder &SetInstances(EBufferUsage usage, T &&src) {
        SetInstances({usage, std::forward<T>(src)}, src.size());
        return *this;
    }

    TMeshBuilder &AddLayout(EDataType dataType, unsigned count) {
        AddLayout(dataType, count, 0);
        return *this;
    }
};

class TInstanceMeshBuilder {
public:
    BUILDER_PROPERTY2(TArrayBuffer, unsigned, Instances);
    BUILDER_LIST3(EDataType, unsigned, unsigned, Layout);

    template<typename T>
    TInstanceMeshBuilder &SetInstances(EBufferUsage usage, T &&src) {
        SetInstances({usage, std::forward<T>(src)}, src.size());
        return *this;
    }

    TInstanceMeshBuilder &AddLayout(EDataType dataType, unsigned count) {
        AddLayout(dataType, count, 0);
        return *this;
    }
};

class TMesh {
private:
    std::shared_ptr<GLuint> VertexArrayObject;
    TArrayBuffer Vertices;
    TIndexBuffer Indices;
    TArrayBuffer Instances;
    unsigned VertexCount;
    unsigned IndexCount;
    unsigned InstanceCount;
    std::vector<std::tuple<EDataType, unsigned, unsigned>> Layout;

public:
    TMesh(const TMeshBuilder &builder);
    TMesh(const TMesh &mesh, const TInstanceMeshBuilder &builder);
    void Draw(EDrawType type = EDrawType::Triangles) const;

    [[nodiscard]] const TArrayBuffer &GetVertices() const { return Vertices; }
    [[nodiscard]] const TIndexBuffer &GetIndices() const { return Indices; }
    [[nodiscard]] const TArrayBuffer &GetInstances() const { return Instances; }
    [[nodiscard]] unsigned GetVertexCount() const { return VertexCount; }
    [[nodiscard]] unsigned GetIndexCount() const { return IndexCount; }
    [[nodiscard]] unsigned GetInstanceCount() const { return InstanceCount; }
};
