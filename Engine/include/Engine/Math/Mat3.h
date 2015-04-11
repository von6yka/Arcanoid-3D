#pragma once


namespace engine
{


class Mat3
{
public:
    float m[3][3];

public:
    Mat3(float m00, float m01, float m02,
        float m10, float m11, float m12, 
        float m20, float m21, float m22)
    {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
    }

    Mat3(float initializeValue = 0)
    {
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                m[i][j] = initializeValue;
            }
        }
    }

    Mat3 operator + (const Mat3& other) const
    {
        Mat3 result;

        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                result.m[i][j] = m[i][j] + other.m[i][j];
            }
        }

        return result;
    }

    Mat3 operator * (const Mat3& other) const
    {
        Mat3 result;

        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                for (size_t k = 0; k < 3; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }

        return result;
    }

    const float* data() const
    {
        return &m[0][0];
    }

    Vec3 operator * (const Vec3& vec3) const
    {
        Vec3 result;

        for (size_t i = 0; i < 3; ++i) {
            result[i] += m[i][0] * vec3.x;
            result[i] += m[i][1] * vec3.y;
            result[i] += m[i][2] * vec3.z;
        }

        return result;  
    }

    Mat3 inverted() const;

    Mat3 transposed() const;
};


}//namespace engine
