#include "Engine/pch.h"

#include "Engine/Gui/WidgetLayer.h"


namespace engine
{


WidgetLayer::WidgetLayer(Object* parent, const std::wstring& name) :
    Object(parent, name),
    _isVisible(true)
{
}


WidgetLayer::~WidgetLayer()
{
    deleteChildren();
}


void WidgetLayer::addWidget(Widget* widget)
{
    _widgetSet.insert(widget);
    widget->setWidgetLayer(this);
    widget->setParent(this);
}


void WidgetLayer::removeWidget(Widget* widget)
{
    _widgetSet.erase(widget);
    widget->setWidgetLayer(NULL);
    widget->setParent(NULL);
}


void WidgetLayer::update(Window& window, InputSystem& inputSystem, AudioSystem& audioSystem)
{
    WidgetSet::iterator i;
    for (i = _widgetSet.begin(); i != _widgetSet.end(); ++i) {
        Widget* widget = *i;
        if (widget->isVisible()) {
            widget->update(window, inputSystem, audioSystem);
        }
    }
}


}//namespace engine
