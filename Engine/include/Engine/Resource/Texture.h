#pragma once

#include <vector>

#include "Engine/SharedPtr.h"

#include "Engine/Math/Size.h"
#include "Engine/Resource/Resource.h"


namespace engine
{


class Texture : public Resource
{
public:
    void* renderSystemData;

public:
    Texture(ResourceStorage& storage, const std::vector<BYTE>& data, const Size& size) :
        Resource(storage, TYPE_TEXTURE),
        renderSystemData(NULL),
        _data(data),
        _size(size)
    {
    }

    const std::vector<BYTE>& data() const
    {
        return _data;
    }

    Size size() const
    {
        return _size;
    }

private:
    std::vector<BYTE> _data;
    Size _size;
};


}//namespace engine
