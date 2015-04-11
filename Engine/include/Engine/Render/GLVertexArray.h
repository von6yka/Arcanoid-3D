#pragma once

#include "GL/glew.h"

namespace engine
{


class GLVertexArray : NonCopyable
{
public:
    GLVertexArray();
    ~GLVertexArray();

public:
    void bind();
    void unbind();

private:
    GLuint _id;
};


}//namespace engine
