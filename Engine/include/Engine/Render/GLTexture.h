#pragma once

#include <vector>

#include <GL/glew.h>
#include <GL/wglew.h>

#include "Engine/Math/Size.h"


namespace engine
{


class GLTexture : NonCopyable
{
public:
    GLTexture(GLenum target);
    ~GLTexture();

public:
    GLuint id() const;

    void bind(GLenum unit);
    void unbind();

    void loadData(const Size& size, const void* data);

    void setParameter(GLenum name, GLint param);

    static GLuint getCurrentBoundTexture(GLenum target);

private:
    GLenum _target;
    GLuint _id;
};


}//namespace engine
