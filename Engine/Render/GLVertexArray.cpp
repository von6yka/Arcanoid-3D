#include "Engine/pch.h"

#include "Engine/Render/GLVertexArray.h"


namespace engine
{


GLVertexArray::GLVertexArray()
{
    glGenVertexArrays(1, &_id);
}


GLVertexArray::~GLVertexArray()
{
    glDeleteVertexArrays(1, &_id);
}


void GLVertexArray::bind()
{
    glBindVertexArray(_id);
}


void GLVertexArray::unbind()
{
    glBindVertexArray(0);
}


}//namespace engine
