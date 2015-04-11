#pragma once

#include "Engine/Math/Math.h"


namespace engine
{


class Vec2
{
public:
    float x;
    float y;

public:
    Vec2(float x = 0, float y = 0) :
        x(x),
        y(y)
    {

    }

    bool operator == (const Vec2& other) const
    {
        return x == other.x && y == other.y;
    }

    Vec2 operator - (const Vec2& other) const
    {
        return Vec2(x - other.x, y - other.y);
    }

    bool isFuzzyEqual(const Vec2& other) const
    {
        return Math::isFuzzyEqual(x, other.x)
            && Math::isFuzzyEqual(y, other.y);
    }
};


}//namespace engine
