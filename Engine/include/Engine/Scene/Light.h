#pragma once

#include "Engine/Object.h"
#include "Engine/Color.h"
#include "Engine/Math/Vec3.h"


namespace engine
{


class Light : public Object
{
public:
    enum Type {
        Type_Directional,
        Type_Point,
        Type_Spot
    };

    Type type() const
    {
        return _type;
    }
    
    Color ambientColor() const
    {
        return _ambientColor;
    }

    void setAmbientColor(const Color& color)
    {
        _ambientColor = color;
    }

    Color diffuseColor() const
    {
        return _diffuseColor;
    }

    void setDiffuseColor(const Color& color)
    {
        _diffuseColor = color;
    }

    Color specularColor() const
    {
        return _specularColor;
    }

    void setSpecularColor(const Color& color)
    {
        _specularColor = color;
    }

    float shadowDepthBiasAngleCoef() const
    {
        return _shadowDepthBiasAngleCoef;
    }

    void setShadowDepthBiasAngleCoef(float coef)
    {
        _shadowDepthBiasAngleCoef = coef;
    }

    float shadowDepthBias() const
    {
        return _shadowDepthBias;
    }

    void setShadowDepthBias(float bias)
    {
        _shadowDepthBias = bias;
    }

protected:
    Light(Object* parent, Type type) :
        Object(parent),
        _type(type),
        _ambientColor(0.2f),
        _diffuseColor(1.0f),
        _specularColor(1.0f),
        _shadowDepthBiasAngleCoef(0.005f),
        _shadowDepthBias(0.01f)
    {
    }

private:
    Type _type;

    Color _ambientColor;
    Color _diffuseColor;
    Color _specularColor;

    float _shadowDepthBiasAngleCoef;
    float _shadowDepthBias;
};


class DirectionalLight : public Light
{
public:
    DirectionalLight(Object* parent) :
        Light(parent, Light::Type_Directional)
    {
    }
};


class PointLight : public Light
{
public:
    PointLight(Object* parent) :
        Light(parent, Light::Type_Point),
        _range(1024),
        _attenuation(0.1f, 0.005f, 0.00001f)
    {
        setShadowDepthBiasAngleCoef(0.00005f);
        setShadowDepthBias(0.0005f);
    }

    float range() const
    {
        return _range;
    }

    void setRange(float range)
    {
        _range = range;
    }

    Vec3 attenuation() const
    {
        return _attenuation;
    }

    void setAttenuation(const Vec3& attenuation)
    {
        _attenuation = attenuation;
    }

private:
    float _range;

    Vec3 _attenuation;
};


class SpotLight : public Light
{
public:
    SpotLight(Object* parent) :
        Light(parent, Light::Type_Spot),
        _range(1024.0f),
        _angleDegree(30.0f),
        _attenuation(0.1f, 0.005f, 0.00001f)
    {
    }

    float range() const
    {
        return _range;
    }

    void setRange(float range)
    {
        _range = range;
    }

    float angle() const
    {
        return _angleDegree;
    }

    void setAngle(float degree)
    {
        _angleDegree = Math::clamp(degree, 0.0f, 179.0f);
    }

    Vec3 attenuation() const
    {
        return _attenuation;
    }

    void setAttenuation(const Vec3& attenuation)
    {
        _attenuation = attenuation;
    }

private:
    float _range;
    float _angleDegree;

    Vec3 _attenuation;
};


}//namespace engine
