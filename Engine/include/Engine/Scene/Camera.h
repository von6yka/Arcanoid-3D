#pragma once

#include "Engine/Color.h"
#include "Engine/Math/Vec3.h"
#include "Engine/Scene/SceneObject.h"

#undef near
#undef far

namespace engine
{


class Camera : public SceneObject
{
public:
    Camera(SceneObject* parent, const std::wstring& name) :
        SceneObject(parent, name),
        _fov(45.0f),
        _near(5.0f),
        _far(2000.0f)
    {
        setVisible(false);
    }

    float fov() const
    {
        return _fov;
    }

    void setFov(float fov)
    {
        _fov = fov;
    }

    float near() const
    {
        return _near;
    }

    void setNear(float near)
    {
        _near = near;
    }

    void setFar(float far)
    {
        _far = far;
    }

    float far() const
    {
        return _far;
    }

    Color backgroundColor() const
    {
        return _backgroundColor;
    }

    void setBackgroundColor(const Color& color)
    {
        _backgroundColor = color;
    }

private:
    float _fov;
    float _near;
    float _far;

    Color _backgroundColor;
};


}//namespace engine
