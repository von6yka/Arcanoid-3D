#include "Engine/pch.h"

#include "Engine/Display.h"

#include <stdexcept>


namespace engine
{


Size Display::getCurrentResolution()
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    if (width == 0) {
        throw std::runtime_error("GetSystemMetrics return zero width");
    }

    int height = GetSystemMetrics(SM_CYSCREEN);
    if (height == 0) {
        throw std::runtime_error("GetSystemMetrics return zero height");
    }

    return Size(width, height);
}


}//namespace engine
