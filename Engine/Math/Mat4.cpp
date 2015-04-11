#include "Engine/pch.h"

#include <cmath>

#include "Engine/Math/Math.h"
#include "Engine/Math/Mat4.h"


namespace engine
{


Mat4 Mat4::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    const float tx = - (right + left) / (right - left);
    const float ty = - (top + bottom) / (top - bottom);
    const float tz = - (zFar + zNear) / (zFar - zNear);

    const float m00 =  2 / (right - left);
    const float m11 =  2 / (top - bottom);
    const float m22 = -2 / (zFar - zNear);

    return Mat4(
        m00,   0,   0, tx,
          0, m11,   0, ty,
          0,   0, m22, tz,
          0,   0,   0,  1
    );
}


Mat4 Mat4::perspective(float fovYDegree, float aspect, float znear, float zfar)
{
    const float halfFovYRadian = Math::toRadian(fovYDegree * 0.5f);

    const float f = 1 / std::tan(halfFovYRadian);
    const float A = (zfar + znear) / (znear - zfar);
    const float B = (2 * zfar * znear) / (znear - zfar);

    return Mat4(
        f / aspect, 0,  0, 0,
                 0, f,  0, 0,
                 0, 0,  A, B,
                 0, 0, -1, 0
    );
}


Mat4 Mat4::translation(float x, float y, float z)
{
    return Mat4(
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1
    );
}


Mat4 Mat4::translation(const Vec3& vec3)
{
    return translation(vec3.x, vec3.y, vec3.z);
}


Mat4 Mat4::scale(float x, float y, float z)
{
    return Mat4(
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    );
}


Mat4 Mat4::rotationX(float xDegree)
{
    const float xRadian = Math::toRadian(xDegree);

    const float cos = std::cos(xRadian);
    const float sin = std::sin(xRadian);

    return Mat4(
        1,   0,    0, 0,
        0, cos, -sin, 0,
        0, sin,  cos, 0,
        0,   0,    0, 1
    );
}


Mat4 Mat4::rotationY(float yDegree)
{
    const float yRadian = Math::toRadian(yDegree);

    const float cos = std::cos(yRadian);
    const float sin = std::sin(yRadian);

    return Mat4(
        cos,  0, sin, 0,
        0,    1,   0, 0,
        -sin, 0, cos, 0,
        0,    0,   0, 1
    );
}


Mat4 Mat4::rotationZ(float zDegree)
{
    const float zRadian = Math::toRadian(zDegree);

    const float cos = std::cos(zRadian);
    const float sin = std::sin(zRadian);

    return Mat4(
        cos, -sin, 0, 0,
        sin,  cos, 0, 0,
          0,    0, 1, 0,
          0,    0, 0, 1
    );
}


Mat4 Mat4::rotationXYZ(const Vec3& vec3)
{
    return rotationX(vec3.x) * rotationY(vec3.y) * rotationZ(vec3.z);
}


Mat4 Mat4::rotationXZY(const Vec3& vec3)
{
    return rotationX(vec3.x) * rotationZ(vec3.z) * rotationY(vec3.y);
}


Mat4 Mat4::rotationYXZ(const Vec3& vec3)
{
    return rotationY(vec3.y) * rotationX(vec3.x) * rotationZ(vec3.z);
}


Mat4 Mat4::rotationYZX(const Vec3& vec3)
{
    return rotationY(vec3.y) * rotationZ(vec3.z) * rotationX(vec3.x);
}


Mat4 Mat4::rotationZXY(const Vec3& vec3)
{
    return rotationZ(vec3.z) * rotationX(vec3.x) * rotationY(vec3.y);
}


Mat4 Mat4::rotationZYX(const Vec3& vec3)
{
    return rotationZ(vec3.z) * rotationY(vec3.y) * rotationX(vec3.x);
}


}//namespace engine
