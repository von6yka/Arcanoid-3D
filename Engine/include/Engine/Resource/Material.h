#pragma once

#include "Engine/Color.h"
#include "Engine/Resource/Texture.h"


namespace engine
{


class Material
{
public:
    Material() :
        diffuse(1.0f),
        ambient(0.2f),
        specular(1.0f),
        shininess(5.0f)
    {
    }

    Color emission;

    Color diffuse;
    Color ambient;

    Color specular;
    float shininess;

    TexturePtr diffuseTexture;
    TexturePtr normalTexture;
    TexturePtr specularTexture;
};


}//namespace engine
