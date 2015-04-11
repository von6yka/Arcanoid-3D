#pragma once

#include "Engine/Forwards.h"
#include "Engine/SharedPtr.h"


namespace engine
{


class Resource : NonCopyable
{
public:
    enum Type {
        TYPE_TEXTURE,
        TYPE_MESH,
        TYPE_AUDIO_CLIP
    };

public:
    Resource(ResourceStorage& storage, Type type);
    virtual ~Resource();

public:
    Type type() const
    {
        return _type;
    }

private:
    ResourceStorage& _storage;

    Type _type;
};


}//namespace engine
