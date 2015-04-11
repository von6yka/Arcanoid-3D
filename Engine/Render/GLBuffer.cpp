#include "Engine/pch.h"

#include "Engine/Render/GLBuffer.h"


namespace engine
{


GLBuffer::GLBuffer(GLenum target) :
    _target(target)
{
    glGenBuffers(1, &_id);
}


GLBuffer::~GLBuffer()
{
    glDeleteBuffers(1, &_id);
}


void GLBuffer::bind()
{
    glBindBuffer(_target, _id);
}


void GLBuffer::unbind()
{
    glBindBuffer(_target, 0);
}


void GLBuffer::loadData(const void* buffer, size_t bufferSize, GLenum usage)
{
    //FIXME: add assert
    glBufferData(_target, bufferSize, buffer, usage);
}


}//namespace engine
