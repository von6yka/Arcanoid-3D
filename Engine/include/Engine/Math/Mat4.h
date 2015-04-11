#pragma once

#include "Engine/Math/Vec3.h"
#include "Engine/Math/Mat3.h"

namespace engine
{


class Mat4
{
public:
    static Mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar);
    static Mat4 perspective(float fovy, float aspect, float znear, float zfar);

    static Mat4 translation(float x, float y, float z);
    static Mat4 translation(const Vec3& vec3);

    static Mat4 scale(float x, float y, float z);

    static Mat4 Mat4::rotationX(float xDegree);
    static Mat4 Mat4::rotationY(float yDegree);
    static Mat4 Mat4::rotationZ(float zDegree);

    static Mat4 Mat4::rotationXYZ(const Vec3& vec3);
    static Mat4 Mat4::rotationXZY(const Vec3& vec3);
    static Mat4 Mat4::rotationYXZ(const Vec3& vec3);
    static Mat4 Mat4::rotationYZX(const Vec3& vec3);
    static Mat4 Mat4::rotationZXY(const Vec3& vec3);
    static Mat4 Mat4::rotationZYX(const Vec3& vec3);

public:
    float m[4][4];

public:
    Mat4(float m00, float m01, float m02, float m03,
         float m10, float m11, float m12, float m13,
         float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33)
    {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }

    Mat4(float initializeValue = 0)
    {
        for (size_t i = 0; i < 4; ++i) {
            for (size_t j = 0; j < 4; ++j) {
                m[i][j] = initializeValue;
            }
        }
    }

    Mat4 operator + (const Mat4& other) const
    {
        Mat4 result;

        for (size_t i = 0; i < 4; ++i) {
            for (size_t j = 0; j < 4; ++j) {
                result.m[i][j] = m[i][j] + other.m[i][j];
            }
        }

        return result;
    }

    Mat4 operator * (const Mat4& other) const
    {
        Mat4 result;

        for (size_t i = 0; i < 4; ++i) {
            for (size_t j = 0; j < 4; ++j) {
                for (size_t k = 0; k < 4; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }

        return result;
    }

    Vec3 operator * (const Vec3& v) const
    {
        Vec3 result;

        result.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * 1;
        result.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * 1;
        result.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * 1;

        float w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * 1;

        return result / w;
    }

    const float* data() const
    {
        return &m[0][0];
    }

    Mat3 toMat3()
    {
        return Mat3(
            m[0][0], m[0][1], m[0][2],
            m[1][0], m[1][1], m[1][2],
            m[2][0], m[2][1], m[2][2]
        );
    }
};


}//namespace engine
