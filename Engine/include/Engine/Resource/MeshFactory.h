#pragma once

#include "Engine/Forwards.h"

namespace engine
{

class Mesh;

class MeshFactory
{
public:
    static Mesh* allocCuboid(ResourceStorage& storage, float width, float height, float depth, bool shouldInverted);

    static Mesh* allocSphere(ResourceStorage& storage, float radius, unsigned int cPlane);
};


}//namespace engine
