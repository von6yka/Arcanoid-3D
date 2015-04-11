#pragma once

#include "Engine/Interface.h"


namespace engine
{

class Resource;

class ResourceStorageObserver : public Interface
{
public:
    virtual void resourceAdded(Resource& resource) = 0;
    virtual void resourceRemoved(const Resource& resource) = 0;
};


}//namespace engine
