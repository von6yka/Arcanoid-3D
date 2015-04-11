#include "Engine/pch.h"

#include <stdexcept>

#include "Engine/Input/InputSystem.h"


namespace engine
{


InputSystem::InputSystem(HWND hWnd) :
    _repeatDelay(250),
    _repeatSpeed(150)
{
    DWORD value;

    if (SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &value, 0) != FALSE) {
        _repeatDelay += value * 250;
    }

    if (SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &value, 0) != FALSE) {
        _repeatSpeed = 1000 / static_cast<int>(value + 2);
    }

    if (FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&_directInput, NULL))) {
        throw std::runtime_error("Can't create direct input interface");
    }

    if (FAILED(_directInput->CreateDevice(GUID_SysMouse, &_mouse, NULL))) {
        throw std::runtime_error("Can't create mouse device");
    }

    if (FAILED(_mouse->SetDataFormat(&c_dfDIMouse2))) {
        throw std::runtime_error("Can't set mouse data format");
    }

    if (FAILED(_mouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND))) {
        throw std::runtime_error("Can't set mouse cooperative level");
    }

    DIPROPDWORD dipdw;
    dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj        = 0;
    dipdw.diph.dwHow        = DIPH_DEVICE;
    dipdw.dwData            = SAMPLE_BUFFER_SIZE;

    if (FAILED(_mouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph))) {
        throw std::runtime_error("Can't set mouse property");
    }

    _mouse->Acquire();

    if (FAILED(_directInput->CreateDevice(GUID_SysKeyboard, &_keyboard, NULL))) {
        throw std::runtime_error("Can't create keyboard device");
    }

    if (FAILED(_keyboard->SetDataFormat(&c_dfDIKeyboard))) {
        throw std::runtime_error("Can't set keyboard data format");
    }

    if (FAILED(_keyboard->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND))) {
        throw std::runtime_error("Can't set keyboard cooperative level");
    }

    if (FAILED(_keyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph))) {
        throw std::runtime_error("Can't set keyboard property");
    }

    _keyboard->Acquire();
}


InputSystem::~InputSystem()
{
    releaseInput();
}


void InputSystem::releaseInput()
{
    if (_keyboard != NULL) {
        _keyboard->Unacquire();
        _keyboard->Release();
        _keyboard = NULL;
    }

    if (_mouse != NULL) {
        _mouse->Unacquire();
        _mouse->Release();
        _mouse = NULL;
    }

    if (_directInput != NULL) {
        _directInput->Release();
        _directInput = NULL;
    }
}


void InputSystem::update(DWORD tickCount)
{
    _keyClickedMap.clear();
    updateKeyboard(tickCount);

    _mouseButtonClickedMap.clear();
    _mouseMove = Vec3();
    updateMouse();
}


void InputSystem::updateKeyboard(DWORD tickCount)
{
    DIDEVICEOBJECTDATA didod[SAMPLE_BUFFER_SIZE];
    DWORD cElementReaded = SAMPLE_BUFFER_SIZE;

    HRESULT hr = _keyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, &cElementReaded, 0);
    if (SUCCEEDED(hr)) {
        for (DWORD i = 0; i < cElementReaded; ++i) {
            DWORD key = didod[i].dwOfs;

            const DWORD downMask = 0x80;
            if (didod[i].dwData & downMask) {
                _keyClickedMap[key]++;

                _downedKeyTimeToRepeatMap[key] = tickCount + _repeatDelay;
            }
            else {
                _downedKeyTimeToRepeatMap.erase(key);
            }
        }

        std::map<DWORD, DWORD>::iterator i;
        for (i = _downedKeyTimeToRepeatMap.begin(); i != _downedKeyTimeToRepeatMap.end(); ++i) {
            if (tickCount > i->second) {
                DWORD clickCount = (tickCount - i->second) / _repeatSpeed;

                if (clickCount > 0) {
                    _keyClickedMap[i->first] += clickCount;
                    i->second += clickCount * _repeatSpeed;
                }
            }
        }
    }
    else {
        _downedKeyTimeToRepeatMap.clear();

        _keyboard->Acquire();
    }
}


void InputSystem::updateMouse()
{
    DIDEVICEOBJECTDATA didod[SAMPLE_BUFFER_SIZE];
    DWORD cElementReaded = SAMPLE_BUFFER_SIZE;

    HRESULT hr = _mouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, &cElementReaded, 0);
    if (SUCCEEDED(hr)) {
        const DWORD downMask = 0x80;

        for (DWORD i = 0; i < cElementReaded; ++i) {
            float moveDiff = static_cast<float>(static_cast<int>(didod[i].dwData));

            DWORD button = didod[i].dwOfs;

            switch(button) {
                case DIMOFS_BUTTON0:
                case DIMOFS_BUTTON1:
                case DIMOFS_BUTTON2:
                case DIMOFS_BUTTON3:
                case DIMOFS_BUTTON4:
                case DIMOFS_BUTTON5:
                case DIMOFS_BUTTON6:
                case DIMOFS_BUTTON7:
                    if (didod[i].dwData & downMask) {
                        _downedMouseButtonSet.insert(button);
                    }
                    else {
                        if (_downedMouseButtonSet.count(button) > 0) {
                            ++_mouseButtonClickedMap[button];

                            _downedMouseButtonSet.erase(button);
                        }
                    }
                    break;

                case DIMOFS_X:
                    _mouseMove.x += moveDiff;
                    break;

                case DIMOFS_Y:
                    _mouseMove.y -= moveDiff;
                    break;

                case DIMOFS_Z:
                    _mouseMove.z += moveDiff;
                    break;

                default:
                    // nothing
                    break;
            }
        }
    }
    else {
        _downedMouseButtonSet.clear();

        _mouse->Acquire();
    }
}


bool InputSystem::isKeyDown(int key)
{
    return (_downedKeyTimeToRepeatMap.count(key) > 0);
}


bool InputSystem::isKeyClicked(int key, int* count)
{
    if (count != NULL) {
        *count = 0;
    }

    if (_keyClickedMap.count(key) > 0) {
        if (count != NULL) {
            *count = _keyClickedMap[key];
        }

        return true;
    }

    return false;
}


bool InputSystem::isMouseDown(int button)
{
    button += DIMOFS_BUTTON0;
    return (_downedMouseButtonSet.count(button) > 0);
}


bool InputSystem::isMouseClicked(int button, int* count)
{
    button += DIMOFS_BUTTON0;

    if (count != NULL) {
        *count = 0;
    }

    if (_mouseButtonClickedMap.count(button) > 0) {
        if (count != NULL) {
            *count = _mouseButtonClickedMap[button];
        }

        return true;
    }

    return false;
}


bool InputSystem::isMouseMoved(Vec3* move)
{
    *move = Vec3();

    if (_mouseMove != *move) {
        *move = _mouseMove;

        return true;
    }

    return false;
}


}//namespace engine
