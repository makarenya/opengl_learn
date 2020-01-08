#include "mesh.h"
#include "errors.h"

using namespace std;

namespace {
    void FreeVertexArrayObject(GLuint *object) {
        glDeleteVertexArrays(1, object);
        TGlError::Skip();
    }

    GLenum DataSize(EDataType type) {
        switch (type) {
            case EDataType::Byte: return sizeof(GLbyte);
            case EDataType::UByte: return sizeof(GLubyte);
            case EDataType::Short: return sizeof(GLshort);
            case EDataType::UShort: return sizeof(GLushort);
            case EDataType::Int: return sizeof(GLint);
            case EDataType::UInt: return sizeof(GLuint);
            case EDataType::Float: return sizeof(GLfloat);
            case EDataType::Double: return sizeof(GLdouble);
        }
    }

    std::shared_ptr<GLuint> CreateVertexArrayObject(
        const TArrayBuffer &vertices,
        const TIndexBuffer &indices,
        const TArrayBuffer &instances,
        const std::initializer_list<const std::vector<std::tuple<EDataType, unsigned, unsigned>> *> &layouts) {
        GLuint vao;
        GL_ASSERT(glGenVertexArrays(1, &vao));
        try {
            GL_ASSERT(glBindVertexArray(vao));
            GLubyte *vertexOffset = nullptr;
            GLubyte *instanceOffset = nullptr;
            size_t vertexStride = 0;
            size_t instanceStride = 0;
            for (auto &layout : layouts) {
                for (auto[dataType, count, divisor] : *layout) {
                    (divisor > 0 ? instanceStride : vertexStride) += DataSize(dataType) * count;
                }
            }
            int location = 0;
            for (auto &layout : layouts) {
                for (auto[dataType, count, divisor] : *layout) {
                    if (divisor > 0) {
                        TArrayBinder instanceBinder(instances);
                        GL_ASSERT(glVertexAttribPointer(location, count, static_cast<GLenum>(dataType),
                                                        GL_FALSE, instanceStride, instanceOffset));
                        instanceOffset += count * DataSize(dataType);
                    } else {
                        TArrayBinder arrayBinder(vertices);
                        GL_ASSERT(glVertexAttribPointer(location, count, static_cast<GLenum>(dataType),
                                                        GL_FALSE, vertexStride, vertexOffset));
                        vertexOffset += count * DataSize(dataType);
                    }
                    GL_ASSERT(glVertexAttribDivisor(location, divisor));
                    GL_ASSERT(glEnableVertexAttribArray(location));
                    location++;
                }
            }
            TIndexBinder indexBinder(indices);
            GL_ASSERT(glBindVertexArray(0));
        } catch (...) {
            glBindVertexArray(0);
            glDeleteVertexArrays(1, &vao);
            throw;
        }
        return std::shared_ptr<GLuint>(new GLuint(vao), FreeVertexArrayObject);
    }
}

TMesh::TMesh(const TMeshBuilder &builder)
    : VertexArrayObject(CreateVertexArrayObject(std::get<0>(builder.Vertices_),
                                                std::get<0>(builder.Indices_),
                                                std::get<0>(builder.Instances_),
                                                {&builder.Layouts_}))
      , Vertices(std::get<0>(builder.Vertices_))
      , Indices(std::get<0>(builder.Indices_))
      , Instances(std::get<0>(builder.Instances_))
      , VertexCount(std::get<1>(builder.Vertices_))
      , IndexCount(std::get<1>(builder.Indices_))
      , InstanceCount(std::get<1>(builder.Instances_))
      , Layout(builder.Layouts_) {
}

TMesh::TMesh(const TMesh &mesh, const TInstanceMeshBuilder &builder)
    : VertexArrayObject(CreateVertexArrayObject(mesh.Vertices,
                                                mesh.Indices,
                                                std::get<0>(builder.Instances_),
                                                {&mesh.Layout, &builder.Layouts_}))
      , Vertices(mesh.Vertices)
      , Indices(mesh.Indices)
      , Instances(std::get<0>(builder.Instances_))
      , VertexCount(mesh.VertexCount)
      , IndexCount(mesh.IndexCount)
      , InstanceCount(std::get<1>(builder.Instances_))
      , Layout(builder.Layouts_) {
}

void TMesh::Draw(EDrawType type) const {
    GL_ASSERT(glBindVertexArray(*VertexArrayObject));
    auto t = static_cast<GLenum>(type);
    try {
        if (IndexCount == 0) {
            if (InstanceCount > 0) {
                GL_ASSERT(glDrawArraysInstanced(t, 0, VertexCount, InstanceCount));
            } else {
                GL_ASSERT(glDrawArrays(t, 0, VertexCount));
            }
        } else {
            if (InstanceCount > 0) {
                GL_ASSERT(glDrawElementsInstanced(t, IndexCount, GL_UNSIGNED_INT, nullptr, InstanceCount));
            } else {
                GL_ASSERT(glDrawElements(t, IndexCount, GL_UNSIGNED_INT, nullptr));
            }
        }
    } catch (...) {
        glBindVertexArray(0);
        throw;
    }
    GL_ASSERT(glBindVertexArray(0));
}
