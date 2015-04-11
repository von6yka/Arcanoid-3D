#include "Engine/pch.h"

#include <sstream>

#include "Engine/Color.h"
#include "Engine/Window.h"

#include "Engine/Render/GLForwardPathSceneRenderer.h"
#include "Engine/Render/GLShaderProperties.h"

#include "Engine/Scene/Scene.h"

#include "Engine/Render/GLMesh.h"


namespace engine
{


GLForwardPathSceneRenderer::GLForwardPathSceneRenderer(ResourceStorage& resourceStorage, Window& window) :
    _window(window),
    _renderPathNumber(0),
    _shadowMapGenerator(resourceStorage)
{
    std::stringstream vertexDefines;
    vertexDefines << "#define VERTEX" << "\n";
    vertexDefines << "#define VERTEX_POSITION " << GLShaderProperties::VERTEX_POSITION << std::endl;
    vertexDefines << "#define VERTEX_TEXCOORD " << GLShaderProperties::VERTEX_TEXCOORD << std::endl;
    vertexDefines << "#define VERTEX_NORMAL " << GLShaderProperties::VERTEX_NORMAL << std::endl;
    vertexDefines << "#define VERTEX_TANGENT " << GLShaderProperties::VERTEX_TANGENT << std::endl;
    vertexDefines << "#define VERTEX_BITANGENT " << GLShaderProperties::VERTEX_BITANGENT << std::endl;

    //point
    _pointLightShader.addShaderSrc(GL_VERTEX_SHADER, vertexDefines.str());
    _pointLightShader.addDefine(GL_VERTEX_SHADER, "POINT_LIGHT");
    _pointLightShader.addShaderFile(GL_VERTEX_SHADER, resourceStorage.basePath() + L"/shaders/Main.glsl");

    _pointLightShader.addDefine(GL_FRAGMENT_SHADER, "FRAGMENT");
    _pointLightShader.addDefine(GL_FRAGMENT_SHADER, "POINT_LIGHT");
    _pointLightShader.addShaderFile(GL_FRAGMENT_SHADER, resourceStorage.basePath() + L"/shaders/Main.glsl");
    _pointLightShader.build();

    //direction
    _directionLightShader.addShaderSrc(GL_VERTEX_SHADER, vertexDefines.str());
    _directionLightShader.addDefine(GL_VERTEX_SHADER, "DIRECTION_LIGHT");
    _directionLightShader.addShaderFile(GL_VERTEX_SHADER, resourceStorage.basePath() + L"/shaders/Main.glsl");

    _directionLightShader.addDefine(GL_FRAGMENT_SHADER, "FRAGMENT");
    _directionLightShader.addDefine(GL_FRAGMENT_SHADER, "DIRECTION_LIGHT");
    _directionLightShader.addShaderFile(GL_FRAGMENT_SHADER, resourceStorage.basePath() + L"/shaders/Main.glsl");
    _directionLightShader.build();

    //spot
    _spotLightShader.addShaderSrc(GL_VERTEX_SHADER, vertexDefines.str());
    _spotLightShader.addDefine(GL_VERTEX_SHADER, "SPOT_LIGHT");
    _spotLightShader.addShaderFile(GL_VERTEX_SHADER, resourceStorage.basePath() + L"/shaders/Main.glsl");

    _spotLightShader.addDefine(GL_FRAGMENT_SHADER, "FRAGMENT");
    _spotLightShader.addDefine(GL_FRAGMENT_SHADER, "SPOT_LIGHT");
    _spotLightShader.addShaderFile(GL_FRAGMENT_SHADER, resourceStorage.basePath() + L"/shaders/Main.glsl");
    _spotLightShader.build();
}


GLForwardPathSceneRenderer::~GLForwardPathSceneRenderer()
{

}


void GLForwardPathSceneRenderer::render(const Scene& scene)
{
    Color color = scene.camera()->backgroundColor();
    glClearColor(color.r(), color.g(), color.b(), color.a());
    glClear(GL_COLOR_BUFFER_BIT);

    _renderPathNumber = 0;

    std::list<SceneObject*> objectWithLightList = scene.objectListWithLight();
    while (!objectWithLightList.empty()) {
        SceneObject* lightedSceneObject = objectWithLightList.front();

        if (lightedSceneObject->isVisible()) {
            renderSceneWithLight(scene, *lightedSceneObject);
        }

        objectWithLightList.pop_front();
        ++_renderPathNumber;
    }
}


void GLForwardPathSceneRenderer::resizeViewport()
{
    GLsizei width = _window.size().width();
    GLsizei height = _window.size().height();

    width = std::max(1, width);
    height = std::max(1, height);

    glViewport(0, 0, width, height);
}


void GLForwardPathSceneRenderer::renderSceneWithLight(const Scene& scene, const SceneObject& lightedSceneObject)
{
    GLShaderProgram& shader = selectShaderAndSetLight(scene, lightedSceneObject);

    setCamera(shader, *scene.camera());

    resizeViewport();

    glEnable(GL_BLEND);

    if (_renderPathNumber > 0) {
        glDepthFunc(GL_LEQUAL);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
    else {
        glDepthFunc(GL_LESS);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    std::set<SceneObject*>::const_iterator i = scene.objectSet().begin();
    for (i; i != scene.objectSet().end(); ++i) {
        SceneObject& sceneObject = *(*i);

        if (sceneObject.isVisible()) {
            Mat4 modelMatrix = Mat4::translation(sceneObject.position()) * sceneObject.orientation().toMat4();
            shader.setUniform("transform.model", modelMatrix);

            Mat3 normalMatrix = modelMatrix.toMat3().inverted().transposed();
            shader.setUniform("transform.normal", normalMatrix);

            setMaterial(shader, sceneObject.material());
            if (!sceneObject.mesh().isNull()) {
                static_cast<GLMesh*>(sceneObject.mesh()->renderSystemData)->render();
            }
        }
    }
}


GLShaderProgram& GLForwardPathSceneRenderer::selectShaderAndSetLight(const Scene& scene, const SceneObject& lightedSceneObject)
{
    GLTexture& depthTexture = _shadowMapGenerator.generate(scene, lightedSceneObject);

    const Light* light = lightedSceneObject.light();

    GLShaderProgram* shader = NULL;
    if (light->type() == Light::Type_Point) {
        _pointLightShader.use();
        shader = &_pointLightShader;

        const PointLight* pointLight = static_cast<const PointLight*>(light);

        shader->setUniform("light.attenuation", pointLight->attenuation());
        shader->setUniform("light.near", 1.0f); //FIXME: range
        shader->setUniform("light.far", 2048.0f);
        shader->setUniform("light.position_worldPosition", lightedSceneObject.position());
    }
    else if (light->type() == Light::Type_Spot) {
        _spotLightShader.use();
        shader = &_spotLightShader;

        const SpotLight* spotLight = static_cast<const SpotLight*>(light);

        //FIXME: get from shadow generator
        Mat4 viewMatrix = lightedSceneObject.orientation().inversed().toMat4() * Mat4::translation(-lightedSceneObject.position());
        Mat4 projectionMatrix = Mat4::perspective(spotLight->angle(), 1.0f, 1.0f, spotLight->range()); //FIXME: range
        Mat4 bias = Mat4::translation(0.5f, 0.5f, 0.5f) * Mat4::scale(0.5f, 0.5f, 0.5f);
        Mat4 transform = bias * projectionMatrix * viewMatrix;
        shader->setUniform("transform.light", transform);

        Vec3 direction = lightedSceneObject.orientation() * SceneObject::DEFAULT_DIRECTION;
        float spotCosCutoff = std::cos(Math::toRadian(spotLight->angle()) * 0.5f);

        shader->setUniform("light.attenuation", spotLight->attenuation());
        shader->setUniform("light.spotDirection", direction);
        shader->setUniform("light.spotCosCutoff", spotCosCutoff);
        shader->setUniform("light.attenuation", spotLight->attenuation());
        shader->setUniform("light.position_worldPosition", lightedSceneObject.position());
    }
    else if (light->type() == Light::Type_Directional) {
        _directionLightShader.use();
        shader = &_directionLightShader;

        //FIXME: get from shadow generator
        Mat4 viewMatrix = lightedSceneObject.orientation().inversed().toMat4() * Mat4::translation(-lightedSceneObject.position());
        Mat4 projectionMatrix = Mat4::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, 1.0f, 2000.0f); //FIXME: range
        Mat4 bias = Mat4::translation(0.5f, 0.5f, 0.5f) * Mat4::scale(0.5f, 0.5f, 0.5f);
        Mat4 transform = bias * projectionMatrix * viewMatrix;
        shader->setUniform("transform.light", transform);

        Vec3 direction = lightedSceneObject.orientation() * SceneObject::DEFAULT_DIRECTION;
        shader->setUniform("light.lightDirection", direction);
    }
    else {
        throw std::runtime_error("Unknown light type");
    }

    shader->bindTexture("light.depthTexture", depthTexture, 3);
    shader->setUniform("light.ambient", light->ambientColor());
    shader->setUniform("light.diffuse", light->diffuseColor());
    shader->setUniform("light.specular", light->specularColor());
    shader->setUniform("light.shadowDepthBiasAngleCoef", light->shadowDepthBiasAngleCoef());
    shader->setUniform("light.shadowDepthBias", light->shadowDepthBias());

    return *shader;
}


void GLForwardPathSceneRenderer::setCamera(GLShaderProgram& shader, const Camera& camera)
{
    shader.setUniform("light.viewPosition_worldSpace", camera.position());

    float aspectRatio = _window.size().width() / static_cast<float>(_window.size().height());

    Mat4 viewMatrix = camera.orientation().toMat4() * Mat4::translation(-camera.position());
    Mat4 perspectiveMatrix = Mat4::perspective(camera.fov(), aspectRatio, camera.near(), camera.far());
    Mat4 viewProjectionMatrix = perspectiveMatrix * viewMatrix;

    shader.setUniform("transform.viewProjection", viewProjectionMatrix);
}


void GLForwardPathSceneRenderer::setMaterial(GLShaderProgram& shader, const Material& material)
{
    if (!material.diffuseTexture.isNull()) {
        GLTexture* texture = static_cast<GLTexture*>(material.diffuseTexture->renderSystemData);
        shader.bindTexture("material.diffuseTexture", *texture, 0);
    }

    if (!material.normalTexture.isNull()) {
        GLTexture* texture = static_cast<GLTexture*>(material.normalTexture->renderSystemData);
        shader.bindTexture("material.normalTexture", *texture, 1);
    }

    if (!material.specularTexture.isNull()) {
        GLTexture* texture = static_cast<GLTexture*>(material.specularTexture->renderSystemData);
        shader.bindTexture("material.specularTexture", *texture, 2);
    }

    shader.setUniform("material.ambient", material.ambient);
    shader.setUniform("material.diffuse", material.diffuse);
    shader.setUniform("material.specular", material.specular);
    if (_renderPathNumber == 0) {
        shader.setUniform("material.emission", material.emission);
    }
    else {
        shader.setUniform("material.emission", Color::BLACK);
    }

    shader.setUniform("material.shininess", material.shininess);
}


}//namespace engine
