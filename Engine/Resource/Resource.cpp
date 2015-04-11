#include "Engine/pch.h"

#include "Engine/Resource/Resource.h"
#include "Engine/Resource/ResourceStorage.h"


namespace engine
{


Resource::Resource(ResourceStorage& storage, Type type) :
    _storage(storage),
    _type(type)
{

}


Resource::~Resource()
{
    _storage.resourceAboutToDelete(*this);
}


}//namespace engine
