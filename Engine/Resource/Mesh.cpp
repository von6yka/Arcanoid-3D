#include "Engine/pch.h"

#include "Engine/Resource/Mesh.h"


namespace engine
{


void Mesh::addTriangle(const Vec3& pos1, const Vec3& pos2, const Vec3& pos3,
                       const Vec2& tex1, const Vec2& tex2, const Vec2& tex3,
                       const Vec3& nor1, const Vec3& nor2, const Vec3& nor3)
{
    Vec3 deltaPos1 = pos2 - pos1;
    Vec3 deltaPos2 = pos3 - pos1;

    Vec2 deltaTex1 = tex2 - tex1;
    Vec2 deltaTex2 = tex3 - tex1;

    float r = 1.0f / (deltaTex1.x * deltaTex2.y - deltaTex1.y * deltaTex2.x);
    Vec3 tan = (deltaPos1 * deltaTex2.y - deltaPos2 * deltaTex1.y) * r;
    Vec3 bitan = (deltaPos2 * deltaTex1.x - deltaPos1 * deltaTex2.x) * r;
    tan.normalize();
    bitan.normalize();

    Vec3 tan1 = tan - nor1 * nor1.dotProduct(tan);
    Vec3 tan2 = tan - nor2 * nor2.dotProduct(tan);
    Vec3 tan3 = tan - nor3 * nor3.dotProduct(tan);
    tan1.normalize();
    tan2.normalize();
    tan3.normalize();

    Vec3 bitan1 = bitan - nor1 * nor1.dotProduct(bitan);
    Vec3 bitan2 = bitan - nor2 * nor2.dotProduct(bitan);
    Vec3 bitan3 = bitan - nor3 * nor3.dotProduct(bitan);
    bitan1.normalize();
    bitan2.normalize();
    bitan3.normalize();

    size_t index1 = addVertex(Vertex(pos1, tex1, nor1, tan1, bitan1));
    size_t index2 = addVertex(Vertex(pos2, tex2, nor2, tan2, bitan2));
    size_t index3 = addVertex(Vertex(pos3, tex3, nor3, tan3, bitan3));

    Triangular triangular = {index1, index2, index3};
    _triangularVector.push_back(triangular);
}


size_t Mesh::addVertex(const Vertex& vertex)
{
    size_t index;

    for (index = 0; index < _vertexVector.size(); ++index) {
        if (_vertexVector[index].isFuzzyEqual(vertex)) {
            break;
        }
    }

    if (index == _vertexVector.size()) {
        _vertexVector.push_back(vertex);
    }

    return index;
}


}//namespace engine
