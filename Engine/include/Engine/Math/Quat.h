#pragma once

#include "Engine/Math/Vec3.h"
#include "Engine/Math/Mat4.h"


namespace engine
{


class Quat
{
public:
    static const Quat ZERO;
    static const Quat IDENTITY;

    static Quat fromAngleAxis(const Vec3& vec3, float angleDegree);

public:
    float x;
    float y;
    float z;
    float w;

public:
    Quat(float x = 0, float y = 0, float z = 0, float w = 1) :
        x(x),
        y(y),
        z(z),
        w(w)
    {
    }

public:
    Quat operator * (const Quat& other) const;

    Vec3 operator * (const Vec3& v) const;

    Quat operator * (const float scalar) const
    {
        return Quat(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    Quat operator / (const float scalar) const
    {
        return Quat(x / scalar, y / scalar, z / scalar, w / scalar);
    }

    float norm() const
    {
        return x * x + y * y + z * z + w * w;
    }

    Quat inversed() const
    {
        float norm = this->norm();

        if (norm > 0.0) {
            return conjugate() / norm;
        }

        return ZERO;
    }

    Quat conjugate() const
    {
        return Quat(-x, -y, -z, w);
    }

    Quat normalized() const
    {
        float len = std::sqrt(norm());
        return Quat(x, y, z, w) / len;
    }

    Mat4 toMat4() const;
};


}//namespace engine
