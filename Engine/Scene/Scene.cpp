#include "Engine/pch.h"

#include "Engine/Scene/Scene.h"


namespace engine
{


Scene::Scene(Object* parent, const std::wstring& name) :
    Object(parent, name)
{

}


Scene::~Scene()
{
    deleteChildren();
}


void Scene::addObject(SceneObject* sceneObject)
{
    _sceneObjectSet.insert(sceneObject);
    sceneObject->setScene(this);
    sceneObject->setParent(this);
}


void Scene::removeObject(SceneObject* sceneObject)
{
    _sceneObjectSet.erase(sceneObject);
    sceneObject->setScene(NULL);
    sceneObject->setParent(NULL);
}


std::list<SceneObject*> Scene::objectListWithLight() const
{
    std::list<SceneObject*> result;

    std::set<SceneObject*>::const_iterator i;
    for (i = _sceneObjectSet.begin(); i != _sceneObjectSet.end(); ++i) {
        SceneObject* sceneObject = *i;
        if (sceneObject->light() != NULL) {
            result.push_back(sceneObject);
        } 
    }

    return result;
}


std::list<SceneObject*> Scene::objectListWithAudioSource() const
{
    std::list<SceneObject*> result;

    std::set<SceneObject*>::const_iterator i;
    for (i = _sceneObjectSet.begin(); i != _sceneObjectSet.end(); ++i) {
        SceneObject* sceneObject = *i;
        if (sceneObject->audioSource() != NULL) {
            result.push_back(sceneObject);
        } 
    }

    return result;
}


}//namespace engine
