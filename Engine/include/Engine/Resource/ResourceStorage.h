#pragma once

#include <list>
#include <map>
#include <string>

#include "Engine/Resource/ResourceStorageObserver.h"
#include "Engine/Resource/Texture.h"
#include "Engine/Resource/AudioClip.h"
#include "Engine/Resource/Mesh.h"

namespace engine
{

class Resource;

class ResourceStorage
{
public:
    ResourceStorage();

    MeshPtr getMesh(const std::wstring& path);
    TexturePtr getTexture(const std::wstring& path);
    AudioClipPtr getAudioClip(const std::wstring& path);

    void addObserver(ResourceStorageObserver& resourceStorageObserver);
    void removeObserver(ResourceStorageObserver& resourceStorageObserver);

    void resourceAboutToDelete(const Resource& resource);

public:
    std::wstring basePath() const
    {
        return _basePath;
    }

private:
    typedef std::list<ResourceStorageObserver*> ObserverList;
    ObserverList _observerList;

    typedef std::map< std::wstring, std::pair<Resource*, ReferenceCounter*> > PathResourceMap;
    PathResourceMap _pathResourceMap;

    std::wstring _basePath;

    MeshPtr loadMesh(const std::wstring& path);
    TexturePtr loadTexture(const std::wstring& path);
    AudioClipPtr loadAudioClip(const std::wstring& path);

    template<typename T>
    SharedPtr<T> getResource(const std::wstring& path)
    {
        SharedPtr<T> concretResource;

        if (_pathResourceMap.count(path) > 0) {
            std::pair<Resource*, ReferenceCounter*> resourceReference = _pathResourceMap[path];

            assert(resourceReference.first != NULL);
            assert(resourceReference.second != NULL);

            concretResource = SharedPtr<T>(static_cast<T*>(resourceReference.first), resourceReference.second);
        }

        return concretResource;
    }

    void storeResource(const std::wstring& path, ResourcePtr resource);
};


}//namespace engine
