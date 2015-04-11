#pragma once

#include <assert.h>
#include <list>
#include <string>

#include <windows.h>

#include "Engine/Forwards.h"
#include "Engine/Object.h"
#include "Engine/UniquePtr.h"

namespace engine
{


class Engine : public Object
{
public:
    Engine(const std::wstring& title);
    ~Engine();

    void exec(EngineController& engineController);

public:
    ResourceStorage& resourceStorage() const
    {
        return *_resourceStorage.get();
    }

    Window& window() const
    {
        return *_window.get();
    }

    InputSystem& inputSystem() const
    {
        return *_inputSystem.get();
    }

    AudioSystem& audioSystem()
    {
        return *_audioSystem.get();
    }

    Scene* scene() const
    {
        return _scene;
    }

    void setScene(Scene* scene)
    {
        _scene = scene;
    }

    void addWidgetLayer(WidgetLayer* widgetLayer);
    void removeWidgetLayer(WidgetLayer* widgetLayer);

private:
    UniquePtr<ResourceStorage> _resourceStorage;

    UniquePtr<Window> _window;

    UniquePtr<GLRenderSystem> _renderSystem;
    UniquePtr<InputSystem> _inputSystem;
    UniquePtr<AudioSystem> _audioSystem;

    WidgetLayer* _debugWidgetLayer;
    Label* _fpsLabel;
    int _fps;
    DWORD _fpsCountingDuration;

    Scene* _scene;

    typedef std::list<WidgetLayer*> WidgetLayerList;
    WidgetLayerList _widgetLayerList;

    void updateFpsCounter(DWORD deltaTime);
    bool update(EngineController& engineController, DWORD tickCount, DWORD deltaTime);
};


}//namespace engine
