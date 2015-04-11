#include "Engine/pch.h"

#include "Engine/Math/Math.h"
#include "Engine/Math/Vec3.h"


namespace engine
{


const float Math::PI = 4.0f * std::atan(1.0f);
const float Math::TWO_PI = 2.0f * Math::PI;
const float Math::HALF_PI = 0.5f * Math::PI;

const float Math::TWO_PI_DEGREE = 360.0f;

bool Math::isIntersect(const Vec3& v1, const Vec3& v2, const Vec3& v3,
    const Vec3& l1, const Vec3& l2, Vec3* p)
{
    const Vec3 nor = Vec3(v2 - v1).crossProduct(Vec3(v3 - v2)).normalized();

    float d1 = Vec3(l1 - v1).dotProduct(nor);
    float d2 = Vec3(l2 - v1).dotProduct(nor);

    if ((d1 > 0.0f && d2 > 0.0f) || (d1 < 0.0f && d2 < 0.0f)) {
        return false;
    }

    *p = l1 + (l2 - l1) * d1 / (std::abs(d1) + std::abs(d2));

    if (Vec3(v2 - v1).crossProduct(Vec3(*p - v1)).dotProduct(nor) < 0) return false;
    if (Vec3(v3 - v2).crossProduct(Vec3(*p - v2)).dotProduct(nor) < 0) return false;
    if (Vec3(v1 - v3).crossProduct(Vec3(*p - v3)).dotProduct(nor) < 0) return false;

    return true;
}


}//namespace engine
