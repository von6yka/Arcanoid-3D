#pragma once

#include <set>
#include <map>

#include "windows.h"

#include "Engine/NonCopyable.h"
#include "Engine/Math/Size.h"

namespace engine
{

class WindowObserver;

class Window : NonCopyable
{
public:
    Window(const std::wstring& title, const Size& size);
    ~Window();

    void show();
    void setFullScreen(bool inFullScreen);
    void setSize(const Size& size);
    Size size() const;

    void recreateWithFormat(int pixelFormat);

    inline HWND handle();
    inline HDC deviceContext();

    void processMessages(bool* quit);

    void addWindowObserver(WindowObserver& windowObserver);
    void removeWindowObserver(WindowObserver& windowObserver);

private:
    static int _cClassReference;
    static std::map<HWND, Window*> _handleWindowMap;

    HWND _handle;
    HDC _deviceContext;

    bool _inFullscreen;

    std::wstring _title;
    Size _size;
    RECT _rect;

    BYTE _bitsPerPel;
    BYTE _depthBits;

    DWORD _style;
    DWORD _exStyle;

    bool _isActive;

    std::set<WindowObserver*> _observerSet;

    static void registerClass();
    static void unregisterClass();

    static LRESULT CALLBACK processMessageStatic(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK processMessage(UINT message, WPARAM wParam, LPARAM lParam);

    void destroy();

    void setRectAndStyle();

    void createWindow();

    void obtainDc();

    void setPixelFormat(int pixelFormat);

    typedef void (WindowObserver::* WindowObserverNotifyFunc)();
    void notifyWindowObserver(WindowObserverNotifyFunc func);
};


inline HWND Window::handle()
{
    return _handle;
}


inline HDC Window::deviceContext()
{
    return _deviceContext;
}


}//namespace engine
