#pragma once

#include "Engine/Forwards.h"

#include "Engine/Callback.h"
#include "Engine/Gui/Widget.h"


namespace engine
{


class Menu : public Widget
{
public:
    Menu(Widget* parent, const std::wstring& name);

public:
    void render(GuiRenderable& guiRenderable) override;

    void update(Window& window, InputSystem& inputSystem, AudioSystem& audioSystem) override;

    void addItem(const std::wstring& text, const CallbackArgs0& clickCallback);

public:
    Color backgroundColor() const
    {
        return _backgroundColor;
    }

    void setBackgroundColor(Color val)
    {
        _backgroundColor = val;
    }

    Color itemTextColor() const
    {
        return _itemTextColor;
    }

    void setItemTextColor(const Color& color)
    {
        _itemTextColor = color;
    }

    Color selectedItemTextColor() const
    {
        return _selectedItemTextColor;
    }

    void setSelectedItemTextColor(const Color& color)
    {
        _selectedItemTextColor = color;
    }

    int padding() const
    {
        return _padding;
    }

    void setPadding(int padding)
    {
        _padding = padding;
    }

    AudioClipPtr onChangeAudioClip() const
    {
        return _onChangeAudioClip;
    }

    void setOnChangeAudioClip(const AudioClipPtr& onChangeAudioClip)
    {
        _onChangeAudioClip = onChangeAudioClip;
    }

    AudioClipPtr onClickAudioClip() const
    {
        return _onClickAudioClip;
    }

    void setOnClickAudioClip(const AudioClipPtr& onClickAudioClip)
    {
        _onClickAudioClip = onClickAudioClip;
    }

private:
    typedef std::list<Label*> LabelList;
    LabelList _labelList;
    std::map<Label*, CallbackArgs0> _itemClickCallbackMap;

    int _padding;

    Color _backgroundColor;
    Color _itemTextColor;
    Color _selectedItemTextColor;

    int _selectedItemNumber;

    AudioClipPtr _onChangeAudioClip;
    AudioClipPtr _onClickAudioClip;

    void handleInput(InputSystem& inputSystem, AudioSystem& audioSystem);

    void updateLayout(Window& window);
};


}//namespace engine
