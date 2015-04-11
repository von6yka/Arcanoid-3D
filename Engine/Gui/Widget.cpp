#include "Engine/pch.h"

#include "Engine/Window.h"
#include "Engine/Gui/Widget.h"
#include "Engine/Gui/WidgetLayer.h"


namespace engine
{


Widget::Widget(Widget* parent, const std::wstring& name) :
    Object(parent, name),
    _widgetLayer(NULL),
    _isVisible(true),
    _anchor(ANCHOR_UNDEFINED)
{
}


Widget::~Widget()
{
    if (_widgetLayer != NULL) {
        _widgetLayer->removeWidget(this);
    }
}


void Widget::update(Window& window, InputSystem&, AudioSystem&)
{
    if (_anchor == ANCHOR_BOTTOM_LEFT) {
        _position = Vec2i(0, 0);
    }
    else if (_anchor == ANCHOR_BOTTOM_RIGHT) {
        _position = Vec2i(window.size().width() - size().width(), 0);
    }
    else if (_anchor == ANCHOR_TOP_LEFT) {
        _position = Vec2i(0, window.size().height() - size().height());
    }
    else if (_anchor == ANCHOR_TOP_RIGHT) {
        _position = Vec2i(
            window.size().width() - size().width(),
            window.size().height() - size().height()
        );
    }
    else if (_anchor == ANCHOR_CENTER) {
        _position = Vec2i(
            (window.size().width() - size().width()) / 2,
            (window.size().height() - size().height()) / 2
        );
    }
}


Size Widget::size() const
{
    return _size;
}


void Widget::setSize(const Size& size)
{
    _size = size;
}


void Widget::setWidgetLayer(WidgetLayer* widgetLayer)
{
    if (_widgetLayer != widgetLayer) {
        WidgetLayer* prevWidgetLayer = _widgetLayer;

        _widgetLayer = widgetLayer;

        if (prevWidgetLayer != NULL) {
            prevWidgetLayer->removeWidget(this);
        }

        if (_widgetLayer != NULL) {
            _widgetLayer->addWidget(this);
        }
    }
}


}//namespace engine
