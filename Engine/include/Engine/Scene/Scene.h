#pragma once

#include <set>
#include <list>

#include "Engine/Object.h"
#include "Engine/Color.h"
#include "Engine/Scene/Camera.h"

namespace engine
{


class SceneObject;


class Scene : public Object
{
public:
    typedef std::set<SceneObject*> SceneObjectSet;
    typedef std::list<SceneObject*> SceneObjectList;

public:
    Scene(Object* parent, const std::wstring& name);
    ~Scene();

    void addObject(SceneObject* sceneObject);
    void removeObject(SceneObject* sceneObject);

    SceneObjectList objectListWithLight() const;
    SceneObjectList objectListWithAudioSource() const;

public:
    SceneObjectSet& objectSet()
    {
        return _sceneObjectSet;
    }

    const SceneObjectSet& objectSet() const
    {
        return _sceneObjectSet;
    }

    Camera* camera() const
    {
        return _camera;
    }

    void setCamera(Camera* camera)
    {
        _camera = camera;
    }

private:
    SceneObjectSet _sceneObjectSet;

    Camera* _camera;
};


}//namespace engine
