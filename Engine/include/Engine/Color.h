#pragma once


namespace engine
{


class Color
{
public:
    static const Color WHITE;
    static const Color BLACK;

public:
    Color() :
        _r(0.0f),
        _g(0.0f),
        _b(0.0f),
        _a(1.0f)
    {}

    Color(float bright, float a = 1.0) :
        _r(bright),
        _g(bright),
        _b(bright),
        _a(a)
    {}

    Color(float r, float g, float b, float a = 1.0) :
        _r(r),
        _g(g),
        _b(b),
        _a(a)
    {}

public:
    float* data()
    {
        return &_r;
    }

    const float* data() const
    {
        return &_r;
    }

    float r() const
    {
        return _r;
    }

    float g() const
    {
        return _g;
    }

    float b() const
    {
        return _b;
    }

    float a() const
    {
        return _a;
    }

    void setR(float r)
    {
        _r = r;
    }

    void setG(float g)
    {
        _g = g;
    }

    void setB(float b)
    {
        _b = b;
    }

    void setA(float a)
    {
        _a = a;
    }

private:
    float _r;
    float _g;
    float _b;
    float _a;
};


}//namespace engine
