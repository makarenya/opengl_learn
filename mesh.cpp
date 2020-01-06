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
}

TMeshBuilder &&TMeshBuilder::Vertices(EBufferUsage usage, const void *data, unsigned length, unsigned count) {
    glGenBuffers(1, &Vbo);
    TGlError::Assert("gen vbo");
    try {
        glBindBuffer(GL_ARRAY_BUFFER, Vbo);
        TGlError::Assert("bind vbo");
        glBufferData(GL_ARRAY_BUFFER, length, data, OpenGlAccess(usage));
        TGlError::Assert("vbo data");
        VertexCount = count;
    } catch (...) {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        throw;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    TGlError::Assert("unbind vbo");
    return std::move(*this);
}

TMeshBuilder &&TMeshBuilder::Indices(EBufferUsage usage, const void *data, unsigned length, unsigned count) {
    glGenBuffers(1, &Ebo);
    TGlError::Assert("gen ebo");
    try {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo);
        TGlError::Assert("bind ebo");
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, length, data, OpenGlAccess(usage));
        TGlError::Assert("ebo data");
        IndexCount = count;
    } catch (...) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        throw;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    TGlError::Assert("unbind ebo");
    return std::move(*this);
}

TMeshBuilder &&TMeshBuilder::Layout(EDataType type, unsigned count) {
    size_t length = count * DataSize(type);
    Locations.emplace_back(std::forward_as_tuple(OpenGlDataType(type), length, count));
    Stride += length;
    return std::move(*this);
}

GLuint TMeshBuilder::BuildVao() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    TGlError::Assert("gen vao");
    try {
        glBindVertexArray(vao);
        TGlError::Assert("bind vao");
        try {
            if (Ebo != 0) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo);
                TGlError::Assert("bind ebo");
            }
            glBindBuffer(GL_ARRAY_BUFFER, Vbo);
            TGlError::Assert("bind vbo");
            GLubyte *offset = nullptr;
            for (size_t i = 0; i < Locations.size(); ++i) {
                GLenum dataType = std::get<0>(Locations[i]);
                size_t length = std::get<1>(Locations[i]);
                size_t count = std::get<2>(Locations[i]);

                glVertexAttribPointer(i, count, dataType, GL_FALSE, Stride, offset);
                TGlError::Assert("vertex attrib pointer");

                glEnableVertexAttribArray(i);
                TGlError::Assert("enable vertex attrib array");
                offset += length;
            }
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
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
      , VertexCount(builder.VertexCount)
      , IndexCount(builder.IndexCount) {
    builder.Vbo = 0;
    builder.Ebo = 0;
}

TMesh::TMesh(TMesh &&src) noexcept
    : Vao(src.Vao)
      , Vbo(src.Vbo)
      , Ebo(src.Ebo)
      , VertexCount(src.VertexCount)
      , IndexCount(src.IndexCount) {
    src.Vao = 0;
    src.Vbo = 0;
    src.Ebo = 0;
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
    TGlError::Skip();
}

void TMesh::Draw(EDrawType type) const {
    glBindVertexArray(Vao);
    TGlError::Assert("bind vao");
    try {
        if (Ebo == 0) {
            glDrawArrays(static_cast<GLenum>(type), 0, VertexCount);
            TGlError::Assert("draw arrays");
        } else {
            glDrawElements(static_cast<GLenum>(type), IndexCount, GL_UNSIGNED_INT, nullptr);
            TGlError::Assert("draw elements");
        }
    } catch (...) {
        glBindVertexArray(0);
        throw;
    }
    glBindVertexArray(0);
    TGlError::Assert("unbind vao");
}

TVertexBufferMapper::TVertexBufferMapper(TMesh &mesh) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh.Vbo);
    TGlError::Assert("bind array buffer while map");
    try {
        Data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        TGlError::Assert("map array buffer");
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
