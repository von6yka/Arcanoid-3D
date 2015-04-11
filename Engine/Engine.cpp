#include "Engine/pch.h"

#include "Engine/Engine.h"

#include <sstream>

#include "Engine/Display.h"
#include "Engine/EngineController.h"
#include "Engine/Color.h"
#include "Engine/Window.h"

#include "Engine/Input/InputSystem.h"

#include "Engine/Gui/WidgetLayer.h"
#include "Engine/Gui/Label.h"

#include "Engine/Render/GLRenderSystem.h"
#include "Engine/Resource/ResourceStorage.h"

#include "Engine/Audio/AudioSystem.h"


namespace engine
{


Engine::Engine(const std::wstring& title) :
    Object(NULL, L"Engine"),
    _fps(0),
    _fpsCountingDuration(0),
    _scene(NULL)
{
    _resourceStorage.reset(new ResourceStorage);

//    _window.reset(new Window(title, Size(800, 600)));

    _window.reset(new Window(title, Display::getCurrentResolution()));
    _window->setFullScreen(true);

    _renderSystem.reset(new GLRenderSystem(*_resourceStorage));

    _renderSystem->setWindow(_window.get());

    _inputSystem.reset(new InputSystem(_window->handle()));
    _audioSystem.reset(new AudioSystem);

    _debugWidgetLayer = new WidgetLayer(this, L"Debug widget layer");
    addWidgetLayer(_debugWidgetLayer);

    _fpsLabel = new Label(NULL, L"FPS label");

    _fpsLabel->setAnchor(Widget::ANCHOR_TOP_LEFT);
    _debugWidgetLayer->addWidget(_fpsLabel);
}


Engine::~Engine()
{
    deleteChildren();
}


void Engine::exec(EngineController& engineController)
{
    engineController.setEngine(this);

    _window->show();

    int lastTickCount = GetTickCount();

    bool shouldQuit;
    do {
        _window->processMessages(&shouldQuit);

        if (!shouldQuit) {
            const DWORD tickCount = timeGetTime();
            const DWORD deltaTime = tickCount - lastTickCount;
            lastTickCount = tickCount;

            updateFpsCounter(deltaTime);

            shouldQuit = update(engineController, tickCount, deltaTime);

        }
    }
    while (!shouldQuit);
}


void Engine::addWidgetLayer(WidgetLayer* widgetLayer)
{
    _widgetLayerList.push_back(widgetLayer);
}


void Engine::removeWidgetLayer(WidgetLayer* widgetLayer)
{
    _widgetLayerList.remove(widgetLayer);
}


bool Engine::update(EngineController& engineController, DWORD tickCount, DWORD deltaTime)
{
    _inputSystem->update(tickCount);
    bool shouldQuit = engineController.exec(deltaTime);

    if (!shouldQuit) {
        WidgetLayerList::iterator i;
        for (i = _widgetLayerList.begin(); i != _widgetLayerList.end(); ++i) {
            WidgetLayer* widgetLayer = *i;
            if (widgetLayer->isVisible()) {
                widgetLayer->update(window(), inputSystem(), audioSystem());
            }
        }

        _audioSystem->update(scene());
        _renderSystem->renderFrame(scene(), _widgetLayerList);
    }

    return shouldQuit;
}


void Engine::updateFpsCounter(DWORD deltaTime)
{
    _fpsCountingDuration += deltaTime;
    ++_fps;

    if (_fpsCountingDuration > 1000) {
        std::wstringstream ss;
        ss << L"FPS: " << (_fps * 1000 / _fpsCountingDuration);
        _fpsLabel->setText(ss.str());

        _fpsCountingDuration = 0;
        _fps = 0;
    }
}


}//namespace engine
