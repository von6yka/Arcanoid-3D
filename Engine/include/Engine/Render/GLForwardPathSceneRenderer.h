#pragma once

#include "Engine/Forwards.h"

#include "Engine/Noncopyable.h"

#include "Engine/Render/GLShaderProgram.h"
#include "Engine/Render/GLShadowMapGenerator.h"

#include "Engine/Resource/ResourceStorage.h"


namespace engine
{


class GLForwardPathSceneRenderer : NonCopyable
{
public:
    GLForwardPathSceneRenderer(ResourceStorage& resourceStorage, Window& window);
    ~GLForwardPathSceneRenderer();

public:
    void render(const Scene& scene);

private:
    Window& _window;

    GLShaderProgram _pointLightShader;
    GLShaderProgram _directionLightShader;
    GLShaderProgram _spotLightShader;

    int _renderPathNumber;

    GLShadowMapGenerator _shadowMapGenerator;

    void resizeViewport();

    void renderSceneWithLight(const Scene& scene, const SceneObject& lightedSceneObject);

    GLShaderProgram& selectShaderAndSetLight(const Scene& scene, const SceneObject& lightedSceneObject);
    void setCamera(GLShaderProgram& shader, const Camera& camera);
    void setMaterial(GLShaderProgram& shader, const Material& material);
};


}//namespace engine
