#pragma once

#include <assert.h>
#include "Engine/Math/Math.h"


namespace engine
{


class Vec3
{
public:
    static const Vec3 ZERO;

public:
    float x;
    float y;
    float z;

public:
    Vec3() :
        x(0),
        y(0),
        z(0)
    {}

    Vec3(float x, float y, float z) :
        x(x),
        y(y),
        z(z)
    {}

public:
    Vec3 operator - () const
    {
        return Vec3(-x, -y, -z);
    }

    Vec3 operator + (const Vec3& other) const
    {
        return Vec3(
            this->x + other.x,
            this->y + other.y,
            this->z + other.z
        );
    }

    Vec3& operator += (const Vec3& other)
    {
        *this = operator+(other);

        return *this;
    }

    Vec3 operator - (const Vec3& other) const
    {
        return Vec3(
            this->x - other.x,
            this->y - other.y,
            this->z - other.z
        );
    }

    Vec3& operator -= (const Vec3& other)
    {
        *this = operator-(other);

        return *this;
    }

    Vec3 operator * (float scalar) const
    {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    Vec3& operator *= (float scalar)
    {
        *this = operator*(scalar);

        return *this;
    }

    Vec3 operator / (float scalar) const
    {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    Vec3& operator /= (float scalar)
    {
        *this = operator/(scalar);

        return *this;
    }

    bool operator == (const Vec3& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator != (const Vec3& other) const
    {
        return !operator==(other);
    }

    float& operator[] (size_t index)
    {
        assert(index < 4);
        return *(&x + index);
    }

    float* data()
    {
        return &x;
    }

    const float* data() const
    {
        return &x;
    }

    bool isFuzzyEqual(const Vec3& other) const
    {
        return Math::isFuzzyEqual(x, other.x)
            && Math::isFuzzyEqual(y, other.y)
            && Math::isFuzzyEqual(z, other.z);
    }

    float dotProduct(const Vec3& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3 crossProduct(const Vec3& other) const
    {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x);
    }

    float length() const
    {
        return sqrt(x * x + y * y + z * z);
    }

    Vec3 normalized() const
    {
        return Vec3(x, y, z) / length();
    }

    void normalize()
    {
        *this = normalized();
    }

    Vec3 reflected(const Vec3& normal) const
    {
        return *this - (normal * dotProduct(normal) * 2);
    }

    void reflect(const Vec3& normal)
    {
        *this = reflected(normal);
    }

    void invert()
    {
        x = -x;
        y = -y;
        z = -z;
    }

    bool isNaN() const
    {
        return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z);
    }

    bool isInf() const
    {
        return Math::isInf(x) || Math::isInf(y) || Math::isInf(z);
    }

    bool isFinite() const
    {
        return !isInf() && !isNaN();
    }
};


}//namespace engine
