#include "Engine/pch.h"

#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneObject.h"


namespace engine
{


const Vec3 SceneObject::DEFAULT_DIRECTION(0.0f, 0.0f, -1.0f);


SceneObject::SceneObject(SceneObject* parent, const std::wstring& name) :
    Object(parent, name),
    _scene(NULL),
    _isVisible(true),
    _light(NULL),
    _audioSource(NULL)
{
    _sceneObjectParent = parent;

    if (_sceneObjectParent != NULL) {
        _sceneObjectParent->_sceneObjectChildList.push_back(this);
    }
}


SceneObject::~SceneObject()
{
    if (_scene != NULL) {
        _scene->removeObject(this);
    }

    std::list<SceneObject*>::iterator i;
    for (i = _sceneObjectChildList.begin(); i != _sceneObjectChildList.end(); ++i) {
        SceneObject* object = *i;
        object->_sceneObjectParent = NULL;
    }

    if (_sceneObjectParent != NULL) {
        _sceneObjectParent->_sceneObjectChildList.remove(this);
    }
}


void SceneObject::setScene(Scene* scene)
{
    if (_scene != scene) {
        Scene* prevScene = _scene;

        _scene = scene;

        if (prevScene != NULL) {
            prevScene->removeObject(this);
        }

        if (_scene != NULL) {
            _scene->addObject(this);
        }
    }
}


engine::Vec3 SceneObject::position() const
{
    if (_sceneObjectParent != NULL) {
        return _sceneObjectParent->position() + _position;
    }

    return _position;
}


bool SceneObject::isVisible() const
{
    if (_sceneObjectParent != NULL) {
        return _sceneObjectParent->isVisible() && _isVisible;
    }

    return _isVisible;
}


}//namespace engine
