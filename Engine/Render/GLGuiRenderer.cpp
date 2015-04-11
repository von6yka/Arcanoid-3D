#include "Engine/pch.h"

#include <sstream>

#include "Engine/Window.h"

#include "Engine/Math/Vec2.h"
#include "Engine/Resource/ResourceStorage.h"

#include "Engine/Render/GLGuiRenderer.h"
#include "Engine/Render/GLShaderProperties.h"


namespace engine
{


static const Size glyphSize(28, 38); //FIXME:


GLGuiRenderer::GLGuiRenderer(ResourceStorage& resourceStorage, Window& window) :
    _window(window),
    _textAtlasTexture(GL_TEXTURE_2D),
    _textVertexBuffer(GL_ARRAY_BUFFER),
    _rectVertexBuffer(GL_ARRAY_BUFFER)
{
    //text
    createGliphAtlas();

    std::stringstream ss;
    ss << "#define VERTEX_POSITION " << GLShaderProperties::VERTEX_POSITION << std::endl;
    ss << "#define VERTEX_COORD " << GLShaderProperties::VERTEX_TEXCOORD << std::endl;

    _textShader.addDefine(GL_VERTEX_SHADER, "VERTEX");
    _textShader.addShaderSrc(GL_VERTEX_SHADER, ss.str());
    _textShader.addShaderFile(GL_VERTEX_SHADER, resourceStorage.basePath() + L"/shaders/Text.glsl");
    _textShader.addDefine(GL_FRAGMENT_SHADER, "FRAGMENT");
    _textShader.addShaderFile(GL_FRAGMENT_SHADER, resourceStorage.basePath() + L"/shaders/Text.glsl");
    _textShader.build();

    _textVertexArray.bind();

    const GLsizei stride = 4 * sizeof(float);

    _textVertexBuffer.bind();
    glVertexAttribPointer(GLShaderProperties::VERTEX_POSITION, 2, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(GLShaderProperties::VERTEX_POSITION);

    glVertexAttribPointer(GLShaderProperties::VERTEX_TEXCOORD, 2, GL_FLOAT, GL_FALSE, stride, (char*)NULL + 2 * sizeof(float));
    glEnableVertexAttribArray(GLShaderProperties::VERTEX_TEXCOORD);
    _textVertexBuffer.unbind();

    _textVertexArray.unbind();

    //rect
    _rectShader.addDefine(GL_VERTEX_SHADER, "VERTEX");
    _rectShader.addShaderSrc(GL_VERTEX_SHADER, ss.str());
    _rectShader.addShaderFile(GL_VERTEX_SHADER, resourceStorage.basePath() + L"/shaders/Rect.glsl");
    _rectShader.addDefine(GL_FRAGMENT_SHADER, "FRAGMENT");
    _rectShader.addShaderFile(GL_FRAGMENT_SHADER, resourceStorage.basePath() + L"/shaders/Rect.glsl");
    _rectShader.build();

    _rectVertexArray.bind();

    _rectVertexBuffer.bind();
    glVertexAttribPointer(GLShaderProperties::VERTEX_POSITION, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(GLShaderProperties::VERTEX_POSITION);
    _rectVertexBuffer.unbind();

    _rectVertexArray.unbind();
}


GLGuiRenderer::~GLGuiRenderer()
{
}


void GLGuiRenderer::drawRect(const Size& size, const Vec2i& position, const Color& color)
{
    const float rateX = 2.0f / _window.size().width();
    const float rateY = 2.0f / _window.size().height();

    const Vec2 glPosition(position.x * rateX, position.y * rateY);
    const Vec2 glSize(size.width() * rateX, size.height() * rateY);

    float data[4][2] = {
        {-1.0f + glPosition.x,            -1.0f + glPosition.y},
        {-1.0f + glPosition.x + glSize.x, -1.0f + glPosition.y},
        {-1.0f + glPosition.x,            -1.0f + glPosition.y + glSize.y},
        {-1.0f + glPosition.x + glSize.x, -1.0f + glPosition.y + glSize.y}
    };

    _rectVertexBuffer.bind();
    _rectVertexBuffer.loadData(data, sizeof(data), GL_STREAM_DRAW);
    _rectVertexBuffer.unbind();

    _rectShader.use();
    _rectShader.setUniform("rectColor", color);

    _rectVertexArray.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    _rectVertexArray.unbind();
}


void GLGuiRenderer::drawText(const std::wstring& text, const Vec2i& position, const Color& textColor) //FIXME: add font size
{
    if (text.empty()) {
        return;
    }

    //FIXME: Unicode
    size_t i;
    const size_t BUFFER_SIZE = 4096;    
    char mbTextBuffer[BUFFER_SIZE];
    wcstombs_s(&i, mbTextBuffer, sizeof(mbTextBuffer), text.c_str(), text.size());
    std::string mbText(mbTextBuffer);
    //

    loadTextVertex(mbText, position);

    _textShader.use();
    _textShader.bindTexture("atlas", _textAtlasTexture, 0);
    _textShader.setUniform("textColor", textColor);

    _textVertexArray.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, mbText.size() * 4);
    _textVertexArray.unbind();
}


void GLGuiRenderer::createGliphAtlas()
{
    _textAtlasTexture.bind(0);

    _textAtlasTexture.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    _textAtlasTexture.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    _textAtlasTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    _textAtlasTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint prevUnpackAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevUnpackAlignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 16 * glyphSize.width(), 16 * glyphSize.height(), 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    HDC hDC = CreateCompatibleDC(wglGetCurrentDC());

    HBITMAP hbitmap = CreateCompatibleBitmap(hDC, 1, 1);
    SelectObject(hDC, hbitmap);

    HFONT font = CreateFontW(48, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_DONTCARE, L"Courier New");
    SelectObject(hDC, font);

    const MAT2 mat2 = {{0,1}, {0,0}, {0,0}, {0,1}};
    std::vector<BYTE> glyphBuffer;
    std::vector<BYTE> glGlyphBuffer;

    UINT ch = 0;
    for (int row = 0; row < 16; ++row) {
        for (int column = 0; column < 16; ++column) {
            GLYPHMETRICS metrics;
            DWORD bufferSize = GetGlyphOutlineW(hDC, ch, GGO_GRAY8_BITMAP, &metrics, 0, 0, &mat2);

            if (bufferSize == GDI_ERROR) {
                throw std::runtime_error("GetGlyphOutlineW failed");
            }

            if (bufferSize > 0) {
                glyphBuffer.resize(bufferSize);
                bufferSize = GetGlyphOutlineW(hDC, ch, GGO_GRAY8_BITMAP, &metrics, glyphBuffer.size(), (DWORD*)&glyphBuffer[0], &mat2);

                if (bufferSize == GDI_ERROR) {
                    throw std::runtime_error("GetGlyphOutlineW failed");
                }

                const int ROW_SIZE = bufferSize / metrics.gmBlackBoxY;

                glGlyphBuffer.resize(metrics.gmBlackBoxX * metrics.gmBlackBoxY);
                for (UINT y = 0; y < metrics.gmBlackBoxY; ++y) {
                    memcpy(&glGlyphBuffer[0] + metrics.gmBlackBoxX * y, &glyphBuffer[0] + bufferSize - ROW_SIZE * (y + 1), metrics.gmBlackBoxX);
                }

                glTexSubImage2D(GL_TEXTURE_2D, 0, column * glyphSize.width(), row * glyphSize.height(),
                    metrics.gmBlackBoxX, metrics.gmBlackBoxY, GL_RED, GL_UNSIGNED_BYTE, &glGlyphBuffer[0]);
            }

            ++ch;
        }
    }

    DeleteObject(font);
    DeleteObject(hbitmap);
    DeleteDC(hDC);

    glPixelStorei(GL_UNPACK_ALIGNMENT, prevUnpackAlignment);
}


void GLGuiRenderer::loadTextVertex(const std::string& text, const Vec2i& position)  //FIXME: optimize
{
    const float rateX = 2.0f / _window.size().width();
    const float rateY = 2.0f / _window.size().height();

    const Vec2 glPosition(position.x * rateX, position.y * rateY);
    const Vec2 glGlyphSize(glyphSize.width() * rateX, glyphSize.height() * rateY);

    const Vec2 glGlyphSizeCoord(28.0f / 448.0f, 36.0f / 608.0f);

    std::vector<float> vectorData;
    vectorData.reserve(text.size() * 8 * 2);
    float xOffset = 0;

    for (size_t i = 0; i < text.size(); ++i) {
        unsigned char letterCode = text[i];

        const float colomn = (letterCode % 16) / 16.0f;
        const float row = (letterCode / 16) / 16.0f;

        const float data[8][2] = {
            {-1.0f + xOffset + glPosition.x,                 -1.0f + glPosition.y},
            {colomn, row},
            {-1.0f + xOffset + glPosition.x + glGlyphSize.x, -1.0f + glPosition.y},
            {colomn + glGlyphSizeCoord.x, row},
            {-1.0f + xOffset + glPosition.x,                 -1.0f + glPosition.y + glGlyphSize.y},
            {colomn, row + glGlyphSizeCoord.y},
            {-1.0f + xOffset + glPosition.x + glGlyphSize.x, -1.0f + glPosition.y + glGlyphSize.y},
            {colomn + glGlyphSizeCoord.x, row + glGlyphSizeCoord.y}
        };

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 2; ++j) {
                vectorData.push_back(data[i][j]);
            }
        }

        xOffset += glGlyphSize.x;
    }

    _textVertexBuffer.bind();
    _textVertexBuffer.loadData(&vectorData[0], vectorData.size() * sizeof(float), GL_STREAM_DRAW);
    _textVertexBuffer.unbind();
}


}//namespace engine
