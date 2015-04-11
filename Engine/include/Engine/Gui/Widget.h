#pragma once

#include <string>

#include "Engine/Forwards.h"

#include "Engine/Object.h"
#include "Engine/Math/Size.h"
#include "Engine/Math/Vec2i.h"

#include "Engine/Gui/GuiRenderable.h"

namespace engine
{


class Widget : public Object
{
public:
    enum Anchor {
        ANCHOR_UNDEFINED,
        ANCHOR_BOTTOM_LEFT,
        ANCHOR_BOTTOM_RIGHT,
        ANCHOR_TOP_LEFT,
        ANCHOR_TOP_RIGHT,
        ANCHOR_CENTER
    };

    Widget(Widget* parent, const std::wstring& name);

    ~Widget();

public:
    virtual void render(GuiRenderable& guiRenderable) = 0;
    virtual void update(Window& window, InputSystem& inputSystem, AudioSystem& audioSystem);

    virtual Size size() const;
    virtual void setSize(const Size& size);

public:
    bool isVisible() const
    {
        return _isVisible;
    }

    void setVisible(bool isVisible)
    {
        _isVisible = isVisible;
    }

    Vec2i position() const
    {
        return _position;
    }

    void setPosition(const Vec2i& position)
    {
        _position = position;
    }

    Anchor anchor() const
    {
        return _anchor;
    }

    void setAnchor(Anchor anchor)
    {
        _anchor = anchor;
    }

    WidgetLayer* widgetLayer() const
    {
        return _widgetLayer;
    }

    void setWidgetLayer(WidgetLayer* widgetLayer);

private:
    WidgetLayer* _widgetLayer;

    bool _isVisible;

    Size _size;

    Vec2i _position;
    Anchor _anchor;
};


}//namespace engine
