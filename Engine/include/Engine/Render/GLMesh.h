#pragma once

#include <GL/glew.h>
#include <GL/wglew.h>

#include "Engine/Forwards.h"
#include "Engine/NonCopyable.h"

#include "Engine/Render/GLVertexArray.h"
#include "Engine/Render/GLBuffer.h"


namespace engine
{


class GLMesh : NonCopyable
{
public:
    GLMesh(const Mesh& mesh);
    ~GLMesh();

public:
    void render();

private:
    GLVertexArray _vertexArray;
    GLBuffer _vertexBuffer;
    GLBuffer _indexBuffer;
    GLsizei _count;

    void loadVertexArray(const Mesh& mesh);
    void loadIndexBuffer(const Mesh& mesh);
};


}//namespace engine
