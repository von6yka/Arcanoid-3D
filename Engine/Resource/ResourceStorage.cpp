#include "Engine/pch.h"

#include "Engine/Resource/ResourceStorage.h"

#include <sstream>
#include <fstream>
#include <stdexcept>

#include "Engine/Path.h"
#include "Engine/Math/Size.h"

#include "Engine/Resource/MeshFactory.h"


struct Chunk
{
    DWORD id;
    DWORD size;
};


void loadBmp(const std::wstring& fileName, std::vector<BYTE>* data, engine::Size* size) //FIXME: move
{
    std::ifstream file;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(fileName.c_str(), std::ifstream::binary);

    WORD type;
    file.read((char*)&type, sizeof(type));

    if (type != 'MB') {
        throw std::runtime_error("File is not bmp");
    }

    DWORD fileSize;
    file.read((char*)&fileSize, sizeof(fileSize));

    file.ignore(2 * sizeof(WORD));

    DWORD offsetBits; 
    file.read((char*)&offsetBits, sizeof(offsetBits));

    file.ignore(sizeof(DWORD));

    int width;
    int height;

    file.read((char*)&width, sizeof(width));
    file.read((char*)&height, sizeof(height));

    size->setWidth(width);
    size->setHeight(height);

    file.seekg(offsetBits, std::ios_base::beg);

    std::vector<char>::size_type dataSize = fileSize - offsetBits;
    data->resize(dataSize);
    file.read((char*)&(*data)[0], dataSize);
}


void loadWav(const std::wstring& fileName, std::vector<BYTE>* data, WAVEFORMATEX* format) //FIXME: move
{
    std::ifstream file;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(fileName.c_str(), std::ifstream::binary);

    Chunk chunk1;
    file.read((char*)&chunk1, sizeof(chunk1));

    DWORD WAVEid;
    file.read((char*)&WAVEid, sizeof(WAVEid));

    Chunk chunk2;
    file.read((char*)&chunk2, sizeof(chunk2));
    file.read((char*)format, chunk2.size);

    Chunk chunk3;
    file.read((char*)&chunk3, sizeof(chunk3));

    data->resize(chunk3.size);
    file.read((char*)&(*data)[0], chunk3.size);
}


namespace engine
{


ResourceStorage::ResourceStorage()
{
    _basePath = Path::getExecutableDirPath() + L"/Resources";
}


MeshPtr ResourceStorage::getMesh(const std::wstring& path)
{
    MeshPtr mesh = getResource<Mesh>(path);
    if (mesh.isNull()) {
        mesh = loadMesh(path);
    }

    return mesh;
}


TexturePtr ResourceStorage::getTexture(const std::wstring& path)
{
    TexturePtr texture = getResource<Texture>(path);
    if (texture.isNull()) {
        texture = loadTexture(path);
    }

    return texture;
}


AudioClipPtr ResourceStorage::getAudioClip(const std::wstring& path)
{
    AudioClipPtr audioClip = getResource<AudioClip>(path);
    if (audioClip.isNull()) {
        audioClip = loadAudioClip(path);
    }

    return audioClip;
}


void ResourceStorage::addObserver(ResourceStorageObserver& resourceStorageObserver)
{
    _observerList.push_back(&resourceStorageObserver);
}


void ResourceStorage::removeObserver(ResourceStorageObserver& resourceStorageObserver)
{
    _observerList.remove(&resourceStorageObserver);
}


void ResourceStorage::resourceAboutToDelete(const Resource& resource)
{
    ObserverList::iterator observerIt;
    for (observerIt = _observerList.begin(); observerIt != _observerList.end(); ++observerIt) {
        ResourceStorageObserver* observer = *observerIt;
        observer->resourceRemoved(resource);
    }

    PathResourceMap::iterator i;
    for (i = _pathResourceMap.begin(); i != _pathResourceMap.end(); ++i) {
        if (i->second.first == &resource) {
            _pathResourceMap.erase(i);
            break;
        }
    }
}


MeshPtr ResourceStorage::loadMesh(const std::wstring& path)
{
    MeshPtr mesh;

    const std::wstring CUBE_CUBE = L"std:/mesh/cube";
    const std::wstring CUBE_SPHERE = L"std:/mesh/sphere";

    std::wstringstream ss(path);
    ss.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    if (path.compare(0, CUBE_CUBE.length(), CUBE_CUBE) == 0) {
        float width;
        float height;
        float depth;
        std::wstring shouldInverted;

        ss.ignore(CUBE_CUBE.length());

        ss.ignore(1);
        ss >> width;

        ss.ignore(1);
        ss >> height;

        ss.ignore(1);
        ss >> depth;

        if (ss.good()) {
            ss.ignore(1);
            ss >> shouldInverted;
        }

        mesh.reset(MeshFactory::allocCuboid(*this, width, height, depth, shouldInverted == L"inverted"));
    }
    else if (path.compare(0, CUBE_SPHERE.length(), CUBE_SPHERE) == 0) {
        float radius;
        unsigned int cPlane;

        ss.ignore(CUBE_SPHERE.length());

        ss.ignore(1);
        ss >> radius;

        ss.ignore(1);
        ss >> cPlane;

        mesh.reset(MeshFactory::allocSphere(*this, radius, cPlane));
    }
    else {
        throw std::runtime_error("Unknown mesh type");
    }
    storeResource(path, mesh);

    return mesh;
}


TexturePtr ResourceStorage::loadTexture(const std::wstring& path)
{
    std::vector<BYTE> data;
    Size size;

    loadBmp(_basePath + L"/Images/" + path, &data, &size);

    TexturePtr texture(new Texture(*this, data, size));
    storeResource(path, texture);

    return texture;
}


AudioClipPtr ResourceStorage::loadAudioClip(const std::wstring& path)
{
    std::vector<BYTE> data;
    WAVEFORMATEX format;

    loadWav(_basePath + L"/Sounds/" + path, &data, &format);

    AudioClipPtr audioClip(new AudioClip(*this, data, format));
    storeResource(path, audioClip);

    return audioClip;
}


void ResourceStorage::storeResource(const std::wstring& path, ResourcePtr resource)
{
    _pathResourceMap[path] = std::pair<Resource*, ReferenceCounter*>(resource.get(), resource.referenceCounter());

    ObserverList::iterator i;
    for (i = _observerList.begin(); i != _observerList.end(); ++i) {
        ResourceStorageObserver* observer = *i;
        observer->resourceAdded(*resource);
    }
}


}//namespace engine
