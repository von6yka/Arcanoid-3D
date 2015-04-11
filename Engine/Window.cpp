#include "Engine/pch.h"

#include "Engine/Window.h"
#include "Engine/Display.h"

#include "Engine/WindowObserver.h"


namespace engine
{


static LPCWSTR renderWindowClassName = L"render_window";

std::map<HWND, Window*> Window::_handleWindowMap;
int Window::_cClassReference = false;


Window::Window(const std::wstring& title, const Size& size) :
    _handle(NULL),
    _deviceContext(NULL),
    _inFullscreen(false),
    _title(title),
    _size(size),
    _bitsPerPel(32),
    _depthBits(32),
    _style(0),
    _exStyle(0),
    _isActive(true)
{
    registerClass();
    createWindow();
    obtainDc();
    setPixelFormat(0);
}


Window::~Window()
{
    destroy();
    unregisterClass();
}


void Window::destroy()
{
    if (_deviceContext != NULL) {
        if (0 == ReleaseDC(_handle, _deviceContext)) {
            throw std::runtime_error("Release device context failed");
        }
        _deviceContext = NULL;
    }

    if (_handle != NULL) {
        if (0 == DestroyWindow(_handle)) {
            throw std::runtime_error("Could not release window handle");
        }
        _handle = NULL;
    }
}


void Window::show()
{
    ShowWindow(_handle, SW_SHOW);
    SetForegroundWindow(_handle);
    SetFocus(_handle);
}


void Window::recreateWithFormat(int pixelFormat)
{
    destroy();

    createWindow();
    obtainDc();
    setPixelFormat(pixelFormat);
}


void Window::setFullScreen(const bool inFullScreen)
{
    _inFullscreen = inFullScreen;

    setRectAndStyle();

    if (_inFullscreen) {
        Size size = Display::getCurrentResolution();
        if (size != _size) {
            DEVMODE dmScreenSettings = DEVMODE();

            dmScreenSettings.dmSize = sizeof(dmScreenSettings);
            dmScreenSettings.dmPelsWidth = _size.width();
            dmScreenSettings.dmPelsHeight = _size.height();
            dmScreenSettings.dmBitsPerPel = _bitsPerPel;
            dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

            //to enum resolution use EnumDisplaySettings
            if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
                throw std::runtime_error("Cannot change display settings");
            }
        }
    }
    else {
        ChangeDisplaySettings(NULL, 0);
    }

    ShowCursor(_inFullscreen);
}


void Window::setSize(const Size& size)
{
    _size = size;

    _rect.left = 0;
    _rect.top = 0;
    _rect.right = _size.width();
    _rect.bottom = _size.height();

    if (!AdjustWindowRectEx(&_rect, _style, false, _exStyle)) {
        throw std::runtime_error("Cannot adjust window rect");
    }
}


Size Window::size() const
{
    return _size;
}


void Window::addWindowObserver(WindowObserver& windowObserver)
{
    _observerSet.insert(&windowObserver);
}


void Window::removeWindowObserver(WindowObserver& windowObserver)
{
    _observerSet.erase(&windowObserver);
}


void Window::registerClass()
{
    ++_cClassReference;

    if (_cClassReference == 1) {
        WNDCLASS windowClass;
        windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        windowClass.lpfnWndProc = &Window::processMessageStatic;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = GetModuleHandle(NULL);
        windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.hbrBackground  = NULL;
        windowClass.lpszMenuName = NULL;
        windowClass.lpszClassName = renderWindowClassName;

        if (0 == RegisterClass(&windowClass)) {
            throw std::runtime_error("Failed to register the window class");
        }
    }
}


void Window::unregisterClass()
{
    --_cClassReference;
    
    if (_cClassReference == 0) {
        if (0 == UnregisterClass(renderWindowClassName, GetModuleHandle(NULL))) {
            throw std::runtime_error("Could not unregister class.");
        }
    }
}


void Window::setRectAndStyle()
{
    if (_inFullscreen) {
        _exStyle = WS_EX_APPWINDOW;
        _style = WS_POPUP;
    }
    else {
        _exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        _style = WS_OVERLAPPEDWINDOW;
    }

    setSize(_size);
}


void Window::createWindow()
{
    setRectAndStyle();

    _handle = CreateWindowEx(
        _exStyle, 
        renderWindowClassName, 
        _title.c_str(),
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | _style,
        0,
        0,
        _rect.right - _rect.left,
        _rect.bottom - _rect.top,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (NULL == _handle) {
        throw std::runtime_error("Window creation error");
    }

    _handleWindowMap[_handle] = this;
}


void Window::obtainDc()
{
    _deviceContext = GetDC(_handle);
    if (NULL == _deviceContext) {
        throw std::runtime_error("Can't create a device context");
    }
}


void Window::setPixelFormat(int pixelFormat)
{
    PIXELFORMATDESCRIPTOR pixelFormatDescriptor = PIXELFORMATDESCRIPTOR();

    pixelFormatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pixelFormatDescriptor.nVersion = 1;

    pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
    pixelFormatDescriptor.cColorBits = _bitsPerPel;
    pixelFormatDescriptor.cDepthBits = _depthBits;
    pixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;

    if (0 == pixelFormat) {
        pixelFormat = ChoosePixelFormat(_deviceContext, &pixelFormatDescriptor);
        if (0 == pixelFormat) {
            throw std::runtime_error("Can't find a suitable pixel format");
        }
    }

    if (FALSE == SetPixelFormat(_deviceContext, pixelFormat, &pixelFormatDescriptor)) {
        throw std::runtime_error("Can't Set The PixelFormat");
    }
}


void Window::processMessages(bool* quit)
{
    MSG msg;

    if (quit != NULL) {
        *quit = false;
    }

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            if (quit != NULL) {
                *quit = true;
            }
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


LRESULT CALLBACK Window::processMessageStatic(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    std::map<HWND, Window*>::iterator it = _handleWindowMap.find(handle);
    if (it != _handleWindowMap.end()) {
        return it->second->processMessage(message, wParam, lParam);
    }

    return DefWindowProc(handle, message, wParam, lParam);
}


LRESULT CALLBACK Window::processMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_ACTIVATE:
            {
                if (!HIWORD(wParam)) {
                    _isActive = true; 
                }
                else {
                    _isActive = false;
                }
            }
            break;

        case WM_CLOSE:
            {
                PostQuitMessage(0);
            }
            break;

        case WM_SIZE:
            {
                _size.setWidth(LOWORD(lParam));
                _size.setHeight(HIWORD(lParam));

                notifyWindowObserver(&WindowObserver::onWindowResized);
            }
            break;

        default:
            return DefWindowProc(_handle, message, wParam, lParam);
            break;
    }

    return 0;
}


void Window::notifyWindowObserver(WindowObserverNotifyFunc func)
{
    std::set<WindowObserver*>::iterator it = _observerSet.begin();
    for (it; it != _observerSet.end(); ++it) {
        WindowObserver* windowObserver = *it;
        (windowObserver->*func)();
    }
}


}//namespace engine  
