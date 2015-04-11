#pragma once

#include <string>
#include <list>

#include <GL/glew.h>

#include "Engine/Forwards.h"
#include "Engine/NonCopyable.h"


namespace engine
{


class GLShaderProgram : NonCopyable
{
public:
    GLShaderProgram();
    ~GLShaderProgram();

public:
    void addDefine(GLenum shaderType, const std::string& define);
    void addShaderSrc(GLenum shaderType, const std::string& src);
    void addShaderFile(GLenum shaderType, const std::wstring& filePath);
    void build();

    void use();

    void setUniform(const GLchar* name, float value);
    void setUniform(const GLchar* name, int value);
    void setUniform(const GLchar* name, const Vec3& value);
    void setUniform(const GLchar* name, const Color& value);
    void setUniform(const GLchar* name, const Mat3& value);
    void setUniform(const GLchar* name, const Mat4& value);
    void bindTexture(const GLchar* name, GLTexture& value, GLenum unit);

    static GLuint getCurrentBoundProgram();

private:
    GLuint _program;

    typedef std::map< GLenum, std::list<std::string> >  ShaderDefineListMap;
    ShaderDefineListMap _shaderDefineListMap;

    typedef std::map< GLenum, std::list<std::string> >  ShaderSrcListMap;
    ShaderSrcListMap _shaderSrcListMap;

    typedef std::map< GLenum, std::list<std::wstring> > ShaderFileListMap;
    ShaderFileListMap _shaderFileListMap;

    GLint uniformLocation(const GLchar* name) const;

    GLint getShaderParameter(GLint shader, GLenum name);
    std::string getShaderInformationLog(GLint shader);

    GLint getProgramParameter(GLenum name);
    std::string getProgramInformationLog();
};


}//namespace engine
