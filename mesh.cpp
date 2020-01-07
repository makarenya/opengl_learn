#include <iostream>
#include "mesh.h"
#include "errors.h"

using namespace std;

TMeshBuilder::~TMeshBuilder() {
    if (Vbo != 0) {
        glDeleteBuffers(1, &Vbo);
    }
    if (Ebo != 0) {
        glDeleteBuffers(1, &Ebo);
    }
    if (InstanceVbo != 0) {
        glDeleteBuffers(1, &InstanceVbo);
    }
}

TMeshBuilder &&TMeshBuilder::Vertices(EBufferUsage usage, const void *data, unsigned length, unsigned count) {
    GL_ASSERT(glGenBuffers(1, &Vbo));
    try {
        GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, Vbo));
        GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, length, data, OpenGlAccess(usage)));
        VertexCount = count;
    } catch (...) {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        throw;
    }
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
    return std::move(*this);
}

TMeshBuilder &&TMeshBuilder::Indices(EBufferUsage usage, const void *data, unsigned length, unsigned count) {
    GL_ASSERT(glGenBuffers(1, &Ebo));
    try {
        GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo));
        GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, length, data, OpenGlAccess(usage)));
        IndexCount = count;
    } catch (...) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        throw;
    }
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    return std::move(*this);
}

TMeshBuilder &&TMeshBuilder::Instances(EBufferUsage usage, const void *data, unsigned length, unsigned count) {
    GL_ASSERT(glGenBuffers(1, &InstanceVbo));
    try {
        GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, InstanceVbo));
        GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, length, data, OpenGlAccess(usage)));
        InstanceCount = count;
    } catch (...) {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        throw;
    }
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
    return std::move(*this);
}

TMeshBuilder &&TMeshBuilder::Layout(EDataType type, unsigned count, unsigned divisor) {
    size_t length = count * DataSize(type);
    Locations.emplace_back(std::forward_as_tuple(OpenGlDataType(type), length, count, divisor));
    (divisor == 0 ? Stride : InstanceStride) += length;
    return std::move(*this);
}

GLuint TMeshBuilder::BuildVao() {
    std::cout << "build vao" << std::endl;
    GLuint vao;
    GL_ASSERT(glGenVertexArrays(1, &vao));
    try {
        GL_ASSERT(glBindVertexArray(vao));
        try {
            if (Ebo != 0) {
                GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo));
            }
            GLubyte *offset = nullptr;
            GLubyte *instanceOffset = nullptr;
            int location = 0;
            for (auto[dataType, length, count, divisor] : Locations) {
                if (divisor > 0) {
                    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, InstanceVbo));

                    GL_ASSERT(glVertexAttribPointer(location, count, dataType, GL_FALSE, InstanceStride, instanceOffset));
                    instanceOffset += length;
                } else {
                    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, Vbo));

                    std::cout << "location " << location << " count " << count << " data type " << dataType
                              << " stride " << Stride << " offset " << reinterpret_cast<size_t>(offset) << endl;
                    GL_ASSERT(glVertexAttribPointer(location, count, dataType, GL_FALSE, Stride, offset));
                    offset += length;
                }

                GL_ASSERT(glVertexAttribDivisor(location, divisor));

                GL_ASSERT(glEnableVertexAttribArray(location));
                location++;
            }
            GL_ASSERT(glBindVertexArray(0));
            GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
            if (Ebo != 0) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
        } catch (...) {
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            if (Ebo != 0) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
            throw;
        }
        return vao;
    } catch (...) {
        glDeleteVertexArrays(1, &vao);
        throw;
    }
}

GLenum TMeshBuilder::OpenGlAccess(EBufferUsage usage) {
    switch (usage) {
        case EBufferUsage::Static: return GL_STATIC_DRAW;
        case EBufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
        case EBufferUsage::Stream: return GL_STREAM_DRAW;
    }
}

GLenum TMeshBuilder::OpenGlDataType(EDataType type) {
    switch (type) {
        case EDataType::Byte: return GL_BYTE;
        case EDataType::UByte: return GL_UNSIGNED_BYTE;
        case EDataType::Short: return GL_SHORT;
        case EDataType::UShort: return GL_UNSIGNED_SHORT;
        case EDataType::Int: return GL_INT;
        case EDataType::UInt: return GL_UNSIGNED_INT;
        case EDataType::Float: return GL_FLOAT;
        case EDataType::Double: return GL_DOUBLE;
    }
}

GLenum TMeshBuilder::DataSize(EDataType type) {
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

TMesh::TMesh(TMeshBuilder &&builder)
    : Vao(builder.BuildVao())
      , Vbo(builder.Vbo)
      , Ebo(builder.Ebo)
      , InstanceVbo(builder.InstanceVbo)
      , VertexCount(builder.VertexCount)
      , IndexCount(builder.IndexCount)
      , InstanceCount(builder.InstanceCount) {
    builder.Vbo = 0;
    builder.Ebo = 0;
    builder.InstanceVbo = 0;
}

TMesh::TMesh(TMesh &&src) noexcept
    : Vao(src.Vao)
      , Vbo(src.Vbo)
      , Ebo(src.Ebo)
      , InstanceVbo(src.InstanceVbo)
      , VertexCount(src.VertexCount)
      , IndexCount(src.IndexCount)
      , InstanceCount(src.InstanceCount) {
    src.Vao = 0;
    src.Vbo = 0;
    src.Ebo = 0;
    src.InstanceVbo = 0;
}

TMesh::~TMesh() {
    if (Vbo != 0) {
        glDeleteBuffers(1, &Vbo);
    }
    if (Ebo != 0) {
        glDeleteBuffers(1, &Ebo);
    }
    if (Vao != 0) {
        glDeleteVertexArrays(1, &Vao);
    }
    if (InstanceVbo != 0) {
        glDeleteBuffers(1, &InstanceVbo);
    }
    TGlError::Skip();
}

void TMesh::Draw(EDrawType type) const {
    GL_ASSERT(glBindVertexArray(Vao));
    try {
        if (Ebo == 0) {
            if (InstanceCount > 0) {
                GL_ASSERT(glDrawArraysInstanced(static_cast<GLenum>(type), 0, VertexCount, InstanceCount));
            } else {
                GL_ASSERT(glDrawArrays(static_cast<GLenum>(type), 0, VertexCount));
            }
        } else {
            if (InstanceCount > 0) {
                GL_ASSERT(glDrawElementsInstanced(static_cast<GLenum>(type), IndexCount, GL_UNSIGNED_INT, nullptr, InstanceCount));
            } else {
                GL_ASSERT(glDrawElements(static_cast<GLenum>(type), IndexCount, GL_UNSIGNED_INT, nullptr));
            }
        }
    } catch (...) {
        glBindVertexArray(0);
        throw;
    }
    GL_ASSERT(glBindVertexArray(0));
}

TVertexBufferMapper::TVertexBufferMapper(TMesh &mesh, bool instances) {
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, instances ? mesh.InstanceVbo : mesh.Vbo));
    try {
        Data = GL_ASSERTR(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
    } catch (...) {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        throw;
    }
}

TVertexBufferMapper::TVertexBufferMapper(TVertexBufferMapper &&src) noexcept
    : Data(src.Data) {
    src.Data = nullptr;
}

TVertexBufferMapper::~TVertexBufferMapper() {
    if (Data != nullptr) {
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
