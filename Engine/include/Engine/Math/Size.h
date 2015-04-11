#pragma once

namespace engine
{


class Size
{
public:
    Size(int width = 0, int height = 0) :
        _width(width),
        _height(height)
    {
    }

    int width() const
    {
        return _width;
    }

    int height() const
    {
        return _height;
    }

    void setWidth(int width)
    {
        _width = width;
    }

    void setHeight(int height)
    {
        _height = height;
    }

    bool operator < (const Size& other) const
    {
        if (_width < other._width) {
            return true;
        }
        else if (_width == other._width && _height < other._height) {
            return true;
        }

        return false;
    }

    bool operator == (const Size& size) const
    {
        return _width == size._width && _height == size._height;
    }

    bool operator != (const Size& size) const
    {
        return _width != size._width || _height != size._height;
    }

private:
    int _width;
    int _height;
};


}//namespace engine
