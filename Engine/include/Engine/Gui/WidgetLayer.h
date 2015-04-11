#pragma once

#include <string>
#include <set>

#include "Engine/Object.h"
#include "Engine/Gui/Widget.h"


namespace engine
{


class WidgetLayer : public Object
{
public:
    typedef std::set<Widget*> WidgetSet;

public:
    WidgetLayer(Object* parent, const std::wstring& name);

    ~WidgetLayer();

public:
    void addWidget(Widget* widget);
    void removeWidget(Widget* widget);

    virtual void update(Window& window, InputSystem& inputSystem, AudioSystem& audioSystem);

public:
    const WidgetSet& widgetSet() const
    {
        return _widgetSet;
    }

    WidgetSet& widgetSet()
    {
        return _widgetSet;
    }

    bool isVisible() const
    {
        return _isVisible;
    }

    void setVisible(bool isVisible)
    {
        _isVisible = isVisible;
    }

private:
    WidgetSet _widgetSet;

    bool _isVisible;
};


}//namespace engine
