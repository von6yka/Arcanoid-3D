#include "Engine/pch.h"

#include "Engine/Window.h"

#include "Engine/Gui/Menu.h"
#include "Engine/Gui/Label.h"

#include "Engine/Input/InputSystem.h"
#include "Engine/Audio/AudioSystem.h"


namespace engine
{


Menu::Menu(Widget* parent, const std::wstring& name) :
    Widget(parent, name),
    _padding(10),
    _itemTextColor(Color(1.0f)),
    _selectedItemTextColor(Color(1.0f, 0.0f, 0.0f)),
    _selectedItemNumber(0)
{

}


void Menu::render(GuiRenderable& guiRenderable)
{
    guiRenderable.drawRect(size(), position(), backgroundColor());

    LabelList::iterator i;
    for (i = _labelList.begin(); i != _labelList.end(); ++i) {
        Widget* label = *i;
        label->render(guiRenderable);
    }
}


void Menu::update(Window& window, InputSystem& inputSystem, AudioSystem& audioSystem)
{
    handleInput(inputSystem, audioSystem);
    updateLayout(window);
}


void Menu::addItem(const std::wstring& text, const CallbackArgs0& clickCallback)
{
    Label* label = new Label(this, name() + std::wstring(L", item: ") + text);
    label->setText(text);
    _labelList.push_back(label);
    _itemClickCallbackMap[label] = clickCallback;
}


void Menu::handleInput(InputSystem& inputSystem, AudioSystem& audioSystem)
{
    int count;

    if ((inputSystem.isKeyClicked(DIK_UP, &count) && count > 0)
        || (inputSystem.isKeyClicked(DIK_LEFT, &count) && count > 0))
    {
        _selectedItemNumber -= count;

        if (_selectedItemNumber < 0) {
            _selectedItemNumber %= static_cast<int>(_labelList.size());
            _selectedItemNumber += _labelList.size();
        }

        if (!_onChangeAudioClip.isNull()) {
            audioSystem.play(_onChangeAudioClip);
        }
    }

    if ((inputSystem.isKeyClicked(DIK_DOWN, &count) && count > 0)
        || (inputSystem.isKeyClicked(DIK_RIGHT, &count) && count > 0))
    {
        _selectedItemNumber += count;
        _selectedItemNumber %= _labelList.size();

        if (!_onChangeAudioClip.isNull()) {
            audioSystem.play(_onChangeAudioClip);
        }
    }

    if (inputSystem.isKeyClicked(DIK_RETURN, &count)) {
        LabelList::iterator i = _labelList.begin();
        std::advance(i, _selectedItemNumber);

        if (!_onClickAudioClip.isNull()) {
            audioSystem.play(_onClickAudioClip);
        }

        _itemClickCallbackMap[*i]();
    }
}


void Menu::updateLayout(Window& window)
{
    Size maxLabelSize = (_labelList.size() > 0) ? _labelList.front()->size() : 0;
    {
        LabelList::iterator i;
        for (i = _labelList.begin(); i != _labelList.end(); ++i) {
            Label* label = *i;
            if (maxLabelSize < label->size()) {
                maxLabelSize = label->size();
            }
        }
    }

    setSize(Size(
        2 * _padding + maxLabelSize.width(),
        2 * _padding + (maxLabelSize.height()) * _labelList.size() + _padding * (_labelList.size() - 1)
    ));

    Size windowSize = window.size();

    setPosition(Vec2i(
        (windowSize.width() - size().width()) / 2,
        (windowSize.height() - size().height()) / 2
    ));

    Vec2i itemPosition(position().x + size().width() / 2, position().y + _padding);

    int widgetNumber = _labelList.size() - 1;
    LabelList::reverse_iterator i;
    for (i = _labelList.rbegin(); i != _labelList.rend(); ++i) {
        Label* label = *i;

        label->setPosition(Vec2i(itemPosition.x - label->size().width() / 2, itemPosition.y));
        itemPosition.y += label->size().height() + _padding;

        if (_selectedItemNumber == widgetNumber) { //FIXME: move from here
            label->setTextColor(_selectedItemTextColor);
        }
        else {
            label->setTextColor(_itemTextColor);
        }

        --widgetNumber;
    }
}


}//namespace engine
