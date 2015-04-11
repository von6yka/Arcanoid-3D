#pragma once

#include "Engine/Forwards.h"

#include "Engine/Noncopyable.h"

#include "Engine/Gui/GuiRenderable.h"
#include "Engine/Render/GLShaderProgram.h"
#include "Engine/Render/GLTexture.h"
#include "Engine/Render/GLBuffer.h"
#include "Engine/Render/GLVertexArray.h"


namespace engine
{


class GLGuiRenderer : public GuiRenderable, NonCopyable
{
public:
    GLGuiRenderer(ResourceStorage& resourceStorage, Window& window);
    ~GLGuiRenderer();

    void drawRect(const Size& size, const Vec2i& position, const Color& color) override;
    void drawText(const std::wstring& text, const Vec2i& position, const Color& textColor) override;

private:
    Window& _window;

    GLShaderProgram _textShader;
    GLTexture _textAtlasTexture;
    GLBuffer _textVertexBuffer;
    GLVertexArray _textVertexArray;

    GLShaderProgram _rectShader;
    GLBuffer _rectVertexBuffer;
    GLVertexArray _rectVertexArray;

    void createGliphAtlas();
    void loadTextVertex(const std::string& text, const Vec2i& position);
};


}//namespace engine
