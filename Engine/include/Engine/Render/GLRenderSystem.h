#pragma once

#include <map>
#include <list>

#include <GL/glew.h>
#include <GL/wglew.h>

#include "Engine/Forwards.h"

#include "Engine/UniquePtr.h"
#include "Engine/Noncopyable.h"
#include "Engine/WindowObserver.h"

#include "Engine/Resource/ResourceStorageObserver.h"


namespace engine
{


class GLRenderSystem :
    public WindowObserver,
    public ResourceStorageObserver,
    NonCopyable
{
public:
    GLRenderSystem(ResourceStorage& resourceStorage);
    ~GLRenderSystem();

public:
    // WindowObserver
    void onWindowResized() override;

    // ResourceStorageObserver
    void resourceAdded(Resource& resource) override; 
    void resourceRemoved(const Resource& resource) override;

    void setWindow(Window* window);
    void renderFrame(const Scene* scene, const std::list<WidgetLayer*>& widgetLayerList);

private:
    ResourceStorage& _resourceStorage;

    UniquePtr<GLForwardPathSceneRenderer> _sceneRenderer;
    UniquePtr<GLGuiRenderer> _guiRenderer;

    bool _isVSyncEnabled;

    Window* _window;
    HGLRC _glContext;

    void initializeContext(Window* activeWindow);
    void releaseContext();

    void renderWidgetLayerList(const std::list<WidgetLayer*>& widgetLayerList);
};


}//namespace engine
