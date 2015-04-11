#include "Engine/pch.h"

#include "Engine/Render/GLFrameBuffer.h"
#include "Engine/Render/GLTexture.h"


namespace engine
{


GLFrameBuffer::GLFrameBuffer()
{
    glGenFramebuffers(1, &_id);
}


GLFrameBuffer::~GLFrameBuffer()
{
    glDeleteFramebuffers(1, &_id);
}


void GLFrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
}


void GLFrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void GLFrameBuffer::attach(GLenum attachment, const GLTexture& texture)
{
    glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture.id(), 0);
}


void GLFrameBuffer::attach(GLenum attachment, GLenum textarget, const GLTexture& texture)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textarget, texture.id(), 0);
}


}//namespace engine
