#pragma once

#include <string>

#include "Engine/Interface.h"

#include "Engine/Color.h"
#include "Engine/Math/Size.h"
#include "Engine/Math/Vec2i.h"


namespace engine
{


class GuiRenderable : public Interface
{
public:
    virtual void drawRect(const Size& size, const Vec2i& position, const Color& color) = 0;
    virtual void drawText(const std::wstring& text, const Vec2i& position, const Color& textColor) = 0;
};


}//namespace engine
