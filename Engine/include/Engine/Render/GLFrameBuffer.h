#pragma once

#include "GL/glew.h"

#include "Engine/Forwards.h"


namespace engine
{


class GLFrameBuffer : NonCopyable
{
public:
    GLFrameBuffer();
    ~GLFrameBuffer();

public:
    void bind();
    void unbind();

    void attach(GLenum attachment, const GLTexture& texture);
    void attach(GLenum attachment, GLenum textarget, const GLTexture& texture);

private:
    GLuint _id;
};


}//namespace engine
