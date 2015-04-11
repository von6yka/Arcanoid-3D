#pragma once

#include "Engine/Forwards.h"

#include "Engine/Object.h"
#include "Engine/Math/Mat4.h"
#include "Engine/Math/Quat.h"
#include "Engine/Math/Vec3.h"

#include "Engine/Scene/Light.h"

#include "Engine/Resource/Material.h"
#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/Texture.h"
#include "Engine/Resource/AudioClip.h"


namespace engine
{


class SceneObject : public Object
{
public:
    static const Vec3 DEFAULT_DIRECTION;

public:
    SceneObject(SceneObject* parent, const std::wstring& name);

    ~SceneObject();

public:
    void move(const Vec3& offset)
    {
        _position += offset;
    }

    void setScene(Scene* scene);

public:
    const Quat& orientation() const
    {
        return _orientation;
    }

    void setOrientation(const Quat& orientation) 
    {
        _orientation = orientation;
    }

    Vec3 position() const;

    void setPosition(Vec3 position) 
    {
        _position = position;
    }

    bool isVisible() const;

    void setVisible(bool isVisible)
    {
        _isVisible = isVisible;
    }

    MeshPtr mesh() const
    {
        return _mesh;
    }

    void setMesh(const MeshPtr& mesh) 
    {
        _mesh = mesh;
    }

    const Material& material() const
    {
        return _material;
    }

    void setMaterial(const Material& material)
    {
        _material = material;
    }

    Light* light() const
    {
        return _light;
    }

    void setLight(Light* light)
    {
        _light = light;
    }

    AudioSource* audioSource() const
    {
        return _audioSource;
    }

    void setAudioSource(AudioSource* audioSource)
    {
        _audioSource = audioSource;
    }

private:
    Scene* _scene;

    Quat _orientation;
    Vec3 _position;

    bool _isVisible;
 
    MeshPtr _mesh;
    Material _material;

    Light* _light;
    AudioSource* _audioSource;

    SceneObject* _sceneObjectParent;
    std::list<SceneObject*> _sceneObjectChildList;
};


}//namespace engine
