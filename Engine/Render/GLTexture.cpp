#include "Engine/pch.h"

#include "Engine/Render/GLTexture.h"


namespace engine
{


GLTexture::GLTexture(GLenum target) :
    _target(target)
{  
    glGenTextures(1, &_id);
}


GLTexture::~GLTexture()
{
    glDeleteTextures(1, &_id);
}


GLuint GLTexture::id() const
{
    return _id;
}


void GLTexture::bind(GLenum unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(_target, _id);
}


void GLTexture::unbind()
{
    assert(getCurrentBoundTexture(_target) == _id);

    glBindTexture(_target, 0);
}


void GLTexture::loadData(const Size& size, const void* data)
{
    assert(getCurrentBoundTexture(_target) == _id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, size.width(), size.height(), 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}


void GLTexture::setParameter(GLenum name, GLint param)
{
    assert(getCurrentBoundTexture(_target) == _id);

    glTexParameteri(_target, name, param);
}


GLuint GLTexture::getCurrentBoundTexture(GLenum target)
{
    GLint id = 0;

    switch (target) {
        case GL_TEXTURE_1D:
            glGetIntegerv(GL_TEXTURE_BINDING_1D, &id);
            break;

        case GL_TEXTURE_2D:
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &id);
            break;

        case GL_TEXTURE_3D:
            glGetIntegerv(GL_TEXTURE_BINDING_3D, &id);
            break;

        case GL_TEXTURE_CUBE_MAP:
            glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &id);
            break;

        default:
            throw std::runtime_error("Invalid target");
            break;
    }

    return (GLuint)id;
}


}//namespace engine
