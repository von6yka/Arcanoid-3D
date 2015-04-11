#include "Engine/pch.h"

#include "Engine/Math/Mat3.h"


namespace engine
{


Mat3 Mat3::inverted() const
{
    Mat3 result(
        m[1][1] * m[2][2] - m[1][2] * m[2][1],
        m[1][2] * m[2][0] - m[1][0] * m[2][2],
        m[1][0] * m[2][1] - m[1][1] * m[2][0],

        m[0][2] * m[2][1] - m[0][1] * m[2][2],
        m[0][0] * m[2][2] - m[0][2] * m[2][0],
        m[0][1] * m[2][0] - m[0][0] * m[2][1],

        m[0][1] * m[1][2] - m[0][2] * m[1][1],
        m[0][2] * m[1][0] - m[0][0] * m[1][2],
        m[0][0] * m[1][1] - m[0][1] * m[1][0]
    );

    float det = m[0][0] * result.m[0][0] + m[0][1] * result.m[0][1] + m[0][2] * result.m[0][2];
    if (Math::isFuzzyEqual(det, 0)) {
        return *this;
    }

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result.m[i][j] /= det;
        }
    }

    return result;
}


engine::Mat3 Mat3::transposed() const
{
    return Mat3(
        m[0][0], m[1][0], m[2][0],
        m[0][1], m[1][1], m[2][1],
        m[0][2], m[1][2], m[2][2]
    );
}


}//namespace engine
