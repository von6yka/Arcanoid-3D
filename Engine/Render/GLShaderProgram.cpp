#include "Engine/pch.h"

#include "Engine/Render/GLShaderProgram.h"

#include <vector>
#include <fstream>

#include <windows.h>

#include "Engine/Color.h"
#include "Engine/Render/GLTexture.h"


namespace engine
{


std::vector<char> getFileContent(const std::wstring& filePath) //FIXME: move
{
    std::ifstream file;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(filePath.c_str(), std::ifstream::binary);

    file.seekg(0, file.end);
    size_t size = file.tellg();

    std::vector<char> buffer(size);

    file.seekg(0, file.beg);
    file.read(&buffer[0], size);

    return buffer;
}


GLShaderProgram::GLShaderProgram()
{
    _program = glCreateProgram();
}


GLShaderProgram::~GLShaderProgram()
{
    glDeleteProgram(_program);
}


void GLShaderProgram::addDefine(GLenum shaderType, const std::string& define)
{
    _shaderDefineListMap[shaderType].push_back(define);
}


void GLShaderProgram::addShaderSrc(GLenum shaderType, const std::string& src)
{
    _shaderSrcListMap[shaderType].push_back(src);
}


void GLShaderProgram::addShaderFile(GLenum shaderType, const std::wstring& fileName)
{
    _shaderFileListMap[shaderType].push_back(fileName);
}


void GLShaderProgram::build()
{
    ShaderFileListMap::iterator shaderIterator;
    for (shaderIterator = _shaderFileListMap.begin(); shaderIterator != _shaderFileListMap.end(); ++shaderIterator) {
        GLenum shaderType = shaderIterator->first;

        std::string src;

        std::list<std::string> defineList = _shaderDefineListMap[shaderType];
        std::list<std::string>::iterator defineIterator;
        for (defineIterator = defineList.begin(); defineIterator != defineList.end(); ++defineIterator) {
            src += "#define " + *defineIterator + "\n";
        }

        std::list<std::string> srcList = _shaderSrcListMap[shaderType];
        std::list<std::string>::iterator srcIterator;
        for (srcIterator = srcList.begin(); srcIterator != srcList.end(); ++srcIterator) {
            src += *srcIterator + "\n";
        }

        std::list<std::wstring> fileList = _shaderFileListMap[shaderType];
        std::list<std::wstring>::iterator fileIterator;
        for (fileIterator = fileList.begin(); fileIterator != fileList.end(); ++fileIterator) {
            std::vector<char> fileContent = getFileContent(*fileIterator);
            src.insert(src.length(), &fileContent[0], fileContent.size());
        }

        const char* strings[] = {"#version 330 core\n", src.c_str()};
        GLint sizes[] = {strlen("#version 330 core\n"), src.size()};

        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 2, strings, sizes);
        glCompileShader(shader);

        if (getShaderParameter(shader, GL_COMPILE_STATUS) != GL_TRUE) {
            std::string informationLog = getShaderInformationLog(shader);
            OutputDebugStringA(informationLog.c_str());

            throw std::runtime_error("Compile status error");
        }

        glAttachShader(_program, shader);
        glDeleteShader(shader);
    }

    glLinkProgram(_program);

    if (getProgramParameter(GL_LINK_STATUS) != GL_TRUE) {
        std::string informationLog = getProgramInformationLog();
        OutputDebugStringA(informationLog.c_str());

        throw std::runtime_error("Compile status error");
    }
}


void GLShaderProgram::use()
{
    glUseProgram(_program);
}


GLint GLShaderProgram::getShaderParameter(GLint shader, GLenum name)
{
    GLint parameter;
    glGetShaderiv(shader, name, &parameter);

    return parameter;
}


std::string GLShaderProgram::getShaderInformationLog(GLint shader)
{
    GLint size = getShaderParameter(shader, GL_INFO_LOG_LENGTH);

    if (size > 0) {
        std::vector<char> log(size);
        glGetShaderInfoLog(shader, size, &size, &log[0]);
        return std::string(&log[0], size);
    }

    return std::string();
}


GLint GLShaderProgram::getProgramParameter(GLenum name)
{
    GLint parameter;
    glGetProgramiv(_program, name, &parameter);

    return parameter;
}


std::string GLShaderProgram::getProgramInformationLog()
{
    GLint size = getProgramParameter(GL_INFO_LOG_LENGTH);

    if (size > 0) {
        std::vector<char> log(size);
        glGetProgramInfoLog(_program, size, &size, &log[0]);
        return std::string(&log[0], size);
    }

    return std::string();
}


void GLShaderProgram::setUniform(const GLchar* name, float value)
{
    glUniform1f(uniformLocation(name), value);
}


void GLShaderProgram::setUniform(const GLchar* name, int value)
{
    glUniform1i(uniformLocation(name), value);
}


void GLShaderProgram::setUniform(const GLchar* name, const Vec3& value)
{
    glUniform3fv(uniformLocation(name), 1, value.data());
}


void GLShaderProgram::setUniform(const GLchar* name, const Color& value)
{
    glUniform4fv(uniformLocation(name), 1, value.data());
}


void GLShaderProgram::setUniform(const GLchar* name, const Mat3& value)
{
    glUniformMatrix3fv(uniformLocation(name), 1, GL_TRUE, value.data());
}


void GLShaderProgram::setUniform(const GLchar* name, const Mat4& value)
{
    glUniformMatrix4fv(uniformLocation(name), 1, GL_TRUE, value.data());
}


void GLShaderProgram::bindTexture(const GLchar* name, GLTexture& value, GLenum unit)
{
    value.bind(unit);
    glUniform1i(uniformLocation(name), unit);
}


GLint GLShaderProgram::uniformLocation(const GLchar* name) const
{
    assert(getCurrentBoundProgram() == _program);

    GLint location = glGetUniformLocation(_program, name);
    if (location == -1) {
        throw std::runtime_error("Can't get uniform location");
    }

    return location;
}


GLuint GLShaderProgram::getCurrentBoundProgram()
{
    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    return (GLuint)program;
}


}//namespace engine
