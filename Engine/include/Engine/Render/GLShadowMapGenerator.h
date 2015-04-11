#pragma once

#include "Engine/Forwards.h"

#include "Engine/NonCopyable.h"
#include "Engine/Render/GLShaderProgram.h"
#include "Engine/Render/GLFrameBuffer.h"
#include "Engine/Render/GLTexture.h"


namespace engine
{


class GLShadowMapGenerator : NonCopyable
{
public:
    GLShadowMapGenerator(ResourceStorage& resourceStorage);
    ~GLShadowMapGenerator();

public:
    GLTexture& generate(const Scene& scene, const SceneObject& lightedSceneObject);

private:
    GLShaderProgram _shader;

    GLFrameBuffer _frameBufferCube;
    GLTexture _textureCube;

    GLFrameBuffer _frameBuffer2d;
    GLTexture _texture2d;

    void initializeShadowMapCube();
    void initializeShadowMap2d();

    GLTexture& generatePoint(const Scene& scene, const SceneObject& lightedSceneObject);
    GLTexture& generateSpot(const Scene& scene, const SceneObject& lightedSceneObject);
    GLTexture& generateDirection(const Scene& scene, const SceneObject& lightedSceneObject);

    void renderScene(const Scene& scene, const Mat4& viewProjection);
};


}//namespace engine
