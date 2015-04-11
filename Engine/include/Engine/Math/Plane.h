#pragma once

#include "Engine/Math/Vec3.h"


namespace engine
{


class Plane
{
public:
    Vec3 point[3];

public:
    Plane(const Vec3& point1, const Vec3& point2, const Vec3& point3)
    {
        point[0] = point1;
        point[1] = point2;
        point[2] = point3;
    }

    Vec3 normal() const
    {
        return (point[1] - point[0]).crossProduct(point[2] - point[0]).normalized();
    }
};


}//namespace engine
