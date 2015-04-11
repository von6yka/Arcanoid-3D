#pragma once

#include <map>
#include <set>

#include "Windows.h"
#include "dinput.h"

#include "Engine/NonCopyable.h"
#include "Engine/Math/Vec3.h"

namespace engine
{


class InputSystem : NonCopyable
{
public:
    static const int MOUSE_BUTTON1 = 0;
    static const int MOUSE_BUTTON2 = 1;

public:
    InputSystem(HWND hWnd);
    ~InputSystem();

public:
    void update(DWORD tickCount);

    bool isKeyDown(int key);
    bool isKeyClicked(int key, int* count = NULL);

    bool isMouseDown(int button);
    bool isMouseClicked(int button, int* count = NULL);
    bool isMouseMoved(Vec3* move);

public:
    int repeatDelay() const
    {
        return _repeatDelay;
    }

    void setRepeatDelay(int repeatDelay)
    {
        _repeatDelay = repeatDelay;
    }

    int repeatSpeed() const
    {
        return _repeatSpeed;
    }

    void setRepeatSpeed(int repeatSpeed)
    {
        _repeatSpeed = repeatSpeed;
    }

private:
    static const int SAMPLE_BUFFER_SIZE = 512;

    LPDIRECTINPUT8       _directInput;
    LPDIRECTINPUTDEVICE8 _mouse;
    LPDIRECTINPUTDEVICE8 _keyboard;

    int _repeatDelay;
    int _repeatSpeed;

    std::map<DWORD, int> _keyClickedMap;
    std::map<DWORD, DWORD> _downedKeyTimeToRepeatMap;

    std::map<DWORD, int> _mouseButtonClickedMap;
    std::set<DWORD> _downedMouseButtonSet;
    Vec3 _mouseMove;

    void releaseInput();

    void updateKeyboard(DWORD tickCount);
    void updateMouse();
};


}//namespace engine
