#include "Engine/pch.h"

#include "Engine/Render/GLRenderSystem.h"

#include <stdexcept>
#include <fstream>
#include <sstream>

#include "Engine/Window.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Math/Mat4.h"
#include "Engine/Resource/ResourceStorage.h"

#include "Engine/Render/GLMesh.h"
#include "Engine/Render/GLTexture.h"
#include "Engine/Render/GLForwardPathSceneRenderer.h"
#include "Engine/Render/GLGuiRenderer.h"

#include "Engine/Gui/WidgetLayer.h"
#include "Engine/Gui/Widget.h"



namespace engine
{


GLRenderSystem::GLRenderSystem(ResourceStorage& resourceStorage) :
    _resourceStorage(resourceStorage),
    _isVSyncEnabled(false),
    _window(NULL),
    _glContext(NULL)
{
    _resourceStorage.addObserver(*this);
}


GLRenderSystem::~GLRenderSystem()
{
    releaseContext();
    _resourceStorage.removeObserver(*this);
}


void GLRenderSystem::onWindowResized()
{
    //FIXME:
}


void GLRenderSystem::resourceAdded(Resource& resource)
{
    if (resource.type() == Resource::TYPE_MESH) {
        Mesh& mesh = static_cast<Mesh&>(resource);

        mesh.renderSystemData = new GLMesh(mesh);
    }
    else if (resource.type() == Resource::TYPE_TEXTURE) {
        Texture& texture = static_cast<Texture&>(resource);

        GLTexture* glTexture = new GLTexture(GL_TEXTURE_2D);

        glTexture->bind(0);
        glTexture->setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexture->setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexture->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexture->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        int maxAnisotropy;
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
        glTexture->setParameter(GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

        glTexture->loadData(texture.size(), &texture.data()[0]);
        glTexture->unbind();

        texture.renderSystemData = glTexture;
    }
}


void GLRenderSystem::resourceRemoved(const Resource& resource)
{
    if (resource.type() == Resource::TYPE_MESH) {
        const Mesh& mesh = static_cast<const Mesh&>(resource);

        delete static_cast<GLMesh*>(mesh.renderSystemData);
    }
    else if (resource.type() == Resource::TYPE_TEXTURE) {
        const Texture& texture = static_cast<const Texture&>(resource);

        delete static_cast<GLTexture*>(texture.renderSystemData);
    }
}


void GLRenderSystem::setWindow(Window* window)
{
    if (_window != NULL) {
        _window->removeWindowObserver(*this);
        releaseContext();
    }

    _window = window;

    if (_window != NULL) {
        _window->addWindowObserver(*this);
        initializeContext(_window);
    }
}


void GLRenderSystem::renderFrame(const Scene* scene, const std::list<WidgetLayer*>& widgetLayerList)
{
    if (_window == NULL) {
        throw std::runtime_error("Window not set");
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (scene != NULL) {
        Camera* camera = scene->camera();
        if (camera == NULL) {
            throw std::runtime_error("Camera doesn't exist");
        }

        _sceneRenderer->render(*scene);
    }

    renderWidgetLayerList(widgetLayerList);

    if (!SwapBuffers(_window->deviceContext())) {
        throw std::runtime_error("Cannot swap buffer");
    }
}


void GLRenderSystem::initializeContext(Window* activeWindow)
{
    _glContext = wglCreateContext(activeWindow->deviceContext());
    if (NULL == _glContext) {
        throw std::runtime_error("Can't create a GL rendering context");
    }

    if (FALSE == wglMakeCurrent(activeWindow->deviceContext(), _glContext)) {
        throw std::runtime_error("Can't activate the GL rendering context");
    }

    if (GLEW_OK != glewInit()) {
        throw std::runtime_error("glewInit failed");
    }

    int iAttributes[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 24,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 16,
        WGL_STENCIL_BITS_ARB, 0,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 8, //FIXME: add available multisampling levels detection
        0, 0
    };

    float fAttributes[] = {0, 0};
    int pixelFormat = 0;
    UINT numFormats = 0;
    BOOL valid = wglChoosePixelFormatARB(_window->deviceContext(), iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
    if (valid == FALSE || numFormats == 0) {
        throw std::runtime_error("Cannot choose pixel format arb");
    }

    releaseContext();
    _window->recreateWithFormat(pixelFormat);

    int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    _glContext = wglCreateContextAttribsARB(_window->deviceContext(), 0, attribs);
    if (NULL == _glContext) {
        throw std::runtime_error("Can't create a GL rendering context");
    }

    if (FALSE == wglMakeCurrent(_window->deviceContext(), _glContext)) {
        throw std::runtime_error("Can't activate the GL rendering context");
    }

    glewExperimental = TRUE;
    if (GLEW_OK != glewInit()) {
        throw std::runtime_error("glewInit failed");
    }
    glGetError();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    if (wglSwapIntervalEXT != NULL) {
        wglSwapIntervalEXT(_isVSyncEnabled);
    }

    _sceneRenderer.reset(new GLForwardPathSceneRenderer(_resourceStorage, *_window));
    _guiRenderer.reset(new GLGuiRenderer(_resourceStorage, *_window));
}


void GLRenderSystem::releaseContext()
{
    _guiRenderer.reset();
    _sceneRenderer.reset();

    if (_glContext != NULL) {
        if (!wglMakeCurrent(NULL, NULL)) {
            throw std::runtime_error("Release of DC and RC failed");
        }

        if (!wglDeleteContext(_glContext)) {
            throw std::runtime_error("Release rendering context failed");
        }

        _glContext = NULL;
    }
}


void GLRenderSystem::renderWidgetLayerList(const std::list<WidgetLayer*>& widgetLayerList)
{
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::list<WidgetLayer*>::const_iterator layerIterator;
    for (layerIterator = widgetLayerList.begin(); layerIterator != widgetLayerList.end(); ++layerIterator) {
        WidgetLayer* widgetLayer = *layerIterator;
        
        if (widgetLayer->isVisible()) {
            const std::set<Widget*>& widgetSet = widgetLayer->widgetSet();
            std::set<Widget*>::const_iterator widgetIterator;
            for (widgetIterator = widgetSet.begin(); widgetIterator != widgetSet.end(); ++widgetIterator) {
                Widget* widget = *widgetIterator;
                if (widget->isVisible()) {
                    widget->render(*_guiRenderer);
                }
            }
        }
    }

    glDisable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);
}


}//namespace engine
