#pragma once

#include <string>

#include "Engine/Gui/Widget.h"


namespace engine
{


class Label : public Widget
{
public:
    Label(Widget* parent, const std::wstring& name);

public:
    void render(GuiRenderable& guiRenderable) override;

    Size size() const override;
    void setSize(const Size& size) override;

public:
    std::wstring text() const
    {
        return _text;
    }

    void setText(const std::wstring& text)
    {
        _text = text;
    }

    Color textColor() const
    {
        return _textColor;
    }

    void setTextColor(const Color& textColor)
    {
        _textColor = textColor;
    }

private:
    std::wstring _text;

    Color _textColor;
};


}//namespace engine
