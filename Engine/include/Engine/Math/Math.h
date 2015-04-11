#pragma once

#include <cmath>
#include <algorithm>


namespace engine
{

class Vec3;

class  Math
{
public:
    static const float PI;
    static const float TWO_PI;
    static const float HALF_PI;

    static const float TWO_PI_DEGREE;

    static bool isNaN(float f)
    {
        return f != f;
    }

    static bool isInf(float x)
    {
        return !isNaN(x) && isNaN(x - x);
    }

    static bool isFinite(float x)
    {
        return !isNaN(x) && !isInf(x);
    }

    static bool isFuzzyEqual(float p1, float p2)
    {
        return (std::abs(p1 - p2) <= 0.00001f * std::min(std::abs(p1), std::abs(p2)));
    }

    static int fuzzyCompare(float p1, float p2)
    {
        if (isFuzzyEqual(p1, p2)) {
            return 0;
        }
        else if (p1 > p2) {
            return 1;
        }
        else {
            return -1;
        }
    }

    static float round(float number)
    {
        return number < 0.0f ? std::ceil(number - 0.5f) : std::floor(number + 0.5f);
    }

    static bool isIntersect(const Vec3& v1, const Vec3& v2, const Vec3& v3,
        const Vec3& l1, const Vec3& l2, Vec3* p);

    template<typename T>
    static T clamp(T value, T min, T max)
    {
        if (value > max) {
            value = max;
        }
        else if (value < min) {
            value = min;
        }

        return value;
    }

    static float toRadian(float degree)
    {
        return degree * float(PI) / 180.0f;
    }

    static float ufmod(float x, float y)
    {
        return std::fmod(std::fmod(x, y) + y, y);
    }

    static float toDegree(float radian)
    {
        return radian * 180.0f / float(PI);
    }
};


}//namespace engine
