#include "Engine/pch.h"

#include "Engine/Gui/Label.h"


namespace engine
{


Label::Label(Widget* parent, const std::wstring& name) : 
    Widget(parent, name),
    _textColor(Color::WHITE)
{

}


void Label::render(GuiRenderable& guiRenderable)
{
    guiRenderable.drawText(text(), position(), textColor());
}


Size Label::size() const 
{
    return Size(text().length() * 28, 36); //FIXME:
}


void Label::setSize(const Size& /*size*/)
{
    //not impl
}


}//namespace engine
