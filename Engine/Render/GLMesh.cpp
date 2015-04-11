#include "Engine/pch.h"

#include "Engine/Render/GLMesh.h"
#include "Engine/Render/GLShaderProperties.h"

#include "Engine/Resource/Mesh.h"


namespace engine
{


GLMesh::GLMesh(const Mesh& mesh) :
    _vertexBuffer(GL_ARRAY_BUFFER),
    _indexBuffer(GL_ELEMENT_ARRAY_BUFFER)
{
    loadVertexArray(mesh);
    loadIndexBuffer(mesh);

    _count = mesh.vertexCount();
}


GLMesh::~GLMesh()
{
}


void GLMesh::render()
{
    _vertexArray.bind();
    _indexBuffer.bind();

    glDrawElements(GL_TRIANGLES, _count, GL_UNSIGNED_INT, (void*)0);

    _indexBuffer.unbind();
    _vertexArray.unbind();
}


void GLMesh::loadVertexArray(const Mesh& mesh)
{
    const float* buffer;
    size_t bufferSize;
    mesh.vertexAttributeData(&buffer, &bufferSize);

    _vertexArray.bind();

    _vertexBuffer.bind();
    _vertexBuffer.loadData(buffer, bufferSize, GL_STATIC_DRAW);

    glVertexAttribPointer(GLShaderProperties::VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, mesh.vertexAttributeStride(), 0);
    glEnableVertexAttribArray(GLShaderProperties::VERTEX_POSITION);

    glVertexAttribPointer(GLShaderProperties::VERTEX_TEXCOORD, 2, GL_FLOAT, GL_FALSE, mesh.vertexAttributeStride(), (char*)NULL + 3 * sizeof(buffer[0]));
    glEnableVertexAttribArray(GLShaderProperties::VERTEX_TEXCOORD);

    glVertexAttribPointer(GLShaderProperties::VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, mesh.vertexAttributeStride(), (char*)NULL + 5 * sizeof(buffer[0]));
    glEnableVertexAttribArray(GLShaderProperties::VERTEX_NORMAL);

    glVertexAttribPointer(GLShaderProperties::VERTEX_TANGENT, 3, GL_FLOAT, GL_FALSE, mesh.vertexAttributeStride(), (char*)NULL + 8 * sizeof(buffer[0]));
    glEnableVertexAttribArray(GLShaderProperties::VERTEX_TANGENT);

    glVertexAttribPointer(GLShaderProperties::VERTEX_BITANGENT, 3, GL_FLOAT, GL_FALSE, mesh.vertexAttributeStride(), (char*)NULL + 11 * sizeof(buffer[0]));
    glEnableVertexAttribArray(GLShaderProperties::VERTEX_BITANGENT);

    _vertexBuffer.unbind();
    _vertexArray.unbind();
}


void GLMesh::loadIndexBuffer(const Mesh& mesh)
{
    const unsigned int* buffer;
    size_t bufferSize;
    mesh.indexData(&buffer, &bufferSize);

    _indexBuffer.bind();
    _indexBuffer.loadData(buffer, bufferSize, GL_STATIC_DRAW);
    _indexBuffer.unbind();
}


}//namespace engine
