#pragma once

#include <vector>
#include <list>

#include "Engine/SharedPtr.h"

#include "Engine/Math/Vec2.h"
#include "Engine/Math/Vec3.h"
#include "Engine/Resource/Resource.h"


namespace engine
{


class Vertex
{
public:
    Vec3 pos;
    Vec2 tex;
    Vec3 nor;
    Vec3 tan;
    Vec3 bitan;

public:
    Vertex()
    {}

    Vertex(const Vec3& pos, const Vec2& tex, const Vec3& nor, const Vec3& tan, const Vec3& bitan) :
        pos(pos),
        tex(tex),
        nor(nor),
        tan(tan),
        bitan(bitan)
    {}

    bool isFuzzyEqual(const Vertex& other) const
    {
        return pos.isFuzzyEqual(other.pos)
            && tex.isFuzzyEqual(other.tex)
            && nor.isFuzzyEqual(other.nor)
            && tan.isFuzzyEqual(other.tan)
            && bitan.isFuzzyEqual(other.bitan);
    }
};


class Mesh : public Resource
{
public:
    struct Triangular
    {
        int indexArray[3];
    };

    void* renderSystemData;

public:
    Mesh(ResourceStorage& storage, size_t triangularCount = 0) :
        Resource(storage, TYPE_MESH),
        renderSystemData(NULL),
        _vertexVector(triangularCount),
        _triangularVector(triangularCount)
    {}

public:
    void addTriangle(const Vec3& pos1, const Vec3& pos2, const Vec3& pos3,
        const Vec2& tex1, const Vec2& tex2, const Vec2& tex3,
        const Vec3& nor1, const Vec3& nor2, const Vec3& nor3);

    void triangularPosition(size_t triangularNumber, Vec3* v1, Vec3* v2, Vec3* v3)
    {
        Triangular& triangular = _triangularVector[triangularNumber];

        *v1 = _vertexVector[triangular.indexArray[0]].pos;
        *v2 = _vertexVector[triangular.indexArray[1]].pos;
        *v3 = _vertexVector[triangular.indexArray[2]].pos;
    }

    size_t vertexCount() const
    {
        return _triangularVector.size() * 3;
    }

    size_t triangularCount()
    {
        return _triangularVector.size();
    }

    void vertexAttributeData(const float** buffer, size_t* bufferSize) const
    {
        *buffer = reinterpret_cast<const float*>(&_vertexVector[0]);
        *bufferSize = _vertexVector.size() * sizeof(_vertexVector[0]);
    }

    int vertexAttributeStride() const
    {
        return sizeof(Vertex);
    }

    void indexData(const unsigned int** buffer, size_t* bufferSize) const
    {
        *buffer = reinterpret_cast<const unsigned int*>(&_triangularVector[0]);
        *bufferSize = _triangularVector.size() * sizeof(_triangularVector[0]);
    }

private:
    std::vector<Vertex> _vertexVector;
    std::vector<Triangular> _triangularVector;

    size_t Mesh::addVertex(const Vertex& vertex);
};


}//namespace engine
