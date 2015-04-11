#pragma once

#include "GL/glew.h"

namespace engine
{


class GLBuffer : NonCopyable
{
public:
    GLBuffer(GLenum target);
    ~GLBuffer();

public:
    void bind();
    void unbind();

    void loadData(const void* buffer, size_t bufferSize, GLenum usage);

private:
    GLuint _id;
    GLenum _target;
};


}//namespace engine
