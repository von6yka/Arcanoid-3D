#include "Engine/pch.h"

#include <sstream>

#include "Engine/Render/GLShadowMapGenerator.h"
#include "Engine/Render/GLShaderProperties.h"
#include "Engine/Render/GLMesh.h"

#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneObject.h"

#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/ResourceStorage.h"


namespace engine
{

static const int size = 1024; //FIXME:

GLShadowMapGenerator::GLShadowMapGenerator(ResourceStorage& resourceStorage) :
    _textureCube(GL_TEXTURE_CUBE_MAP),
    _texture2d(GL_TEXTURE_2D)
{
    std::stringstream ss;
    ss << "#define VERTEX_POSITION " << GLShaderProperties::VERTEX_POSITION << std::endl;

    _shader.addDefine(GL_VERTEX_SHADER, "VERTEX");
    _shader.addShaderSrc(GL_VERTEX_SHADER, ss.str());
    _shader.addShaderFile(GL_VERTEX_SHADER, resourceStorage.basePath() + L"/shaders/Shadow.glsl");

    _shader.addDefine(GL_FRAGMENT_SHADER, "FRAGMENT");
    _shader.addShaderFile(GL_FRAGMENT_SHADER, resourceStorage.basePath() + L"/shaders/Shadow.glsl");

    _shader.build();

    initializeShadowMapCube();
    initializeShadowMap2d();
}


GLShadowMapGenerator::~GLShadowMapGenerator()
{
}


GLTexture& GLShadowMapGenerator::generate(const Scene& scene, const SceneObject& lightedSceneObject)
{
    glDisable(GL_BLEND);

    if (lightedSceneObject.light()->type() == Light::Type_Point) {
        return generatePoint(scene, lightedSceneObject);
    }
    else if (lightedSceneObject.light()->type() == Light::Type_Spot) {
        return generateSpot(scene, lightedSceneObject);
    }
    else if (lightedSceneObject.light()->type() == Light::Type_Directional) {
        return generateDirection(scene, lightedSceneObject);
    }

    throw std::runtime_error("Unknown light type");
}


void GLShadowMapGenerator::initializeShadowMapCube()
{
    _textureCube.bind(0);

    _textureCube.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    _textureCube.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    _textureCube.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    _textureCube.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    _textureCube.setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    _textureCube.setParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    _textureCube.setParameter(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    _textureCube.unbind();
}


void GLShadowMapGenerator::initializeShadowMap2d()
{
    _texture2d.bind(0);

    _texture2d.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    _texture2d.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    _texture2d.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    _texture2d.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    _texture2d.setParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    _texture2d.setParameter(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    _texture2d.unbind();

    _frameBuffer2d.bind();
    _frameBuffer2d.attach(GL_DEPTH_ATTACHMENT, _texture2d);
    _frameBuffer2d.unbind();
}


GLTexture& GLShadowMapGenerator::generatePoint(const Scene& scene, const SceneObject& lightedSceneObject)
{  
    const Mat4 projection = Mat4::perspective(90.0f, 1.0, 1.0f, 2048.0f); //FIXME: range
    const Mat4 lightTranslation = Mat4::translation(-lightedSceneObject.position());

    std::vector<Mat4> viewProjectionVector;
    viewProjectionVector.reserve(6);

    viewProjectionVector.push_back(projection * Mat4::rotationY(90.0f) * Mat4::rotationX(180.0f) * lightTranslation);
    viewProjectionVector.push_back(projection * Mat4::rotationY(-90.0f) * Mat4::rotationX(180.0f) * lightTranslation);

    viewProjectionVector.push_back(projection * Mat4::rotationX(-90.0f) * lightTranslation);
    viewProjectionVector.push_back(projection * Mat4::rotationX(90.0f) * lightTranslation);

    viewProjectionVector.push_back(projection * Mat4::rotationY(180.0f) * Mat4::rotationZ(180.0f) * lightTranslation);
    viewProjectionVector.push_back(projection * Mat4::rotationZ(180.0f) * lightTranslation);

    _frameBufferCube.bind();
    for (std::vector<Mat4>::size_type i = 0; i < viewProjectionVector.size(); ++i) {
        _frameBufferCube.attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _textureCube);

        Mat4& viewProjection = viewProjectionVector[i];
        renderScene(scene, viewProjection);
    }
    _frameBufferCube.unbind();

    return _textureCube;
}


GLTexture& GLShadowMapGenerator::generateSpot(const Scene& scene, const SceneObject& lightedSceneObject)
{
    SpotLight* spotLight = static_cast<SpotLight*>(lightedSceneObject.light());

    Mat4 view = lightedSceneObject.orientation().inversed().toMat4() * Mat4::translation(-lightedSceneObject.position());
    Mat4 projection = Mat4::perspective(spotLight->angle(), 1.0f, 1.0f, spotLight->range());
    Mat4 viewProjection = projection * view;

    _frameBuffer2d.bind();
    renderScene(scene, viewProjection);
    _frameBuffer2d.unbind();

    return _texture2d;
}


GLTexture& GLShadowMapGenerator::generateDirection(const Scene& scene, const SceneObject& lightedSceneObject)
{
    Mat4 view = lightedSceneObject.orientation().inversed().toMat4() * Mat4::translation(-lightedSceneObject.position());
    Mat4 projection = Mat4::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, 1.0f, 2000.0f);
    Mat4 viewProjection = projection * view;

    _frameBuffer2d.bind();
    renderScene(scene, viewProjection);
    _frameBuffer2d.unbind();

    return _texture2d;
}


void GLShadowMapGenerator::renderScene(const Scene& scene, const Mat4& viewProjection)
{
    glViewport(0, 0, size, size);
    glClear(GL_DEPTH_BUFFER_BIT);

    _shader.use();

    std::set<SceneObject*>::const_iterator i = scene.objectSet().begin();
    for (i; i != scene.objectSet().end(); ++i) {
        SceneObject* sceneObject = *i;

        if (sceneObject->isVisible() && !sceneObject->mesh().isNull()) {
            Mat4 modelMatrix = Mat4::translation(sceneObject->position()) * sceneObject->orientation().toMat4();
            Mat4 modelViewProjection = viewProjection * modelMatrix;

            _shader.setUniform("modelViewProjection", modelViewProjection);

            static_cast<GLMesh*>(sceneObject->mesh()->renderSystemData)->render();
        }
    }
}


}//namespace engine
