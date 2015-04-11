#include "Engine/pch.h"

#include <cmath>

#include "Engine/Resource/Mesh.h"
#include "Engine/Resource/MeshFactory.h"

/****** Cuboid ********

        2 ------ 6
      / |      / |
    /   |    /   |
  3 ------ 7     |
  |     |  |     |
  |     |  |     |
  |     |  |     |
  |     1 -|---- 5
  |   /    |   /
  | /      | /
  0 ------ 4

**********************/


namespace engine
{


Mesh* MeshFactory::allocCuboid(ResourceStorage& storage,  float width, float height, float depth, bool shouldInverted)
{  
    const Vec3 pos[] = {
        Vec3(-width / 2, -height / 2,  depth / 2),
        Vec3(-width / 2, -height / 2, -depth / 2),
        Vec3(-width / 2,  height / 2, -depth / 2),
        Vec3(-width / 2,  height / 2,  depth / 2),
        Vec3( width / 2, -height / 2,  depth / 2),
        Vec3( width / 2, -height / 2, -depth / 2),
        Vec3( width / 2,  height / 2, -depth / 2),
        Vec3( width / 2,  height / 2,  depth / 2)
    };

    const Vec2 tex[] = {
        Vec2(0.0f, 0.0f),
        Vec2(1.0f, 0.0f),
        Vec2(1.0f, 1.0f),
        Vec2(0.0f, 1.0f)
    };

    const Vec3 nor[] = {
        Vec3( 1.0f,  0.0f,  0.0f),
        Vec3(-1.0f,  0.0f,  0.0f),
        Vec3( 0.0f,  1.0f,  0.0f),
        Vec3( 0.0f, -1.0f,  0.0f),
        Vec3( 0.0f,  0.0f,  1.0f),
        Vec3( 0.0f,  0.0f, -1.0f)
    };

    Mesh* mesh = new Mesh(storage);

    if (shouldInverted) {
        mesh->addTriangle(pos[0], pos[1], pos[2], tex[0], tex[1], tex[2], nor[0], nor[0], nor[0]);
        mesh->addTriangle(pos[0], pos[2], pos[3], tex[0], tex[2], tex[3], nor[0], nor[0], nor[0]);

        mesh->addTriangle(pos[5], pos[4], pos[7], tex[0], tex[1], tex[2], nor[1], nor[1], nor[1]);
        mesh->addTriangle(pos[5], pos[7], pos[6], tex[0], tex[2], tex[3], nor[1], nor[1], nor[1]);

        mesh->addTriangle(pos[0], pos[4], pos[5], tex[0], tex[1], tex[2], nor[2], nor[2], nor[2]);
        mesh->addTriangle(pos[0], pos[5], pos[1], tex[0], tex[2], tex[3], nor[2], nor[2], nor[2]);

        mesh->addTriangle(pos[2], pos[6], pos[7], tex[0], tex[1], tex[2], nor[3], nor[3], nor[3]);
        mesh->addTriangle(pos[2], pos[7], pos[3], tex[0], tex[2], tex[3], nor[3], nor[3], nor[3]);

        mesh->addTriangle(pos[1], pos[5], pos[6], tex[0], tex[1], tex[2], nor[4], nor[4], nor[4]);
        mesh->addTriangle(pos[1], pos[6], pos[2], tex[0], tex[2], tex[3], nor[4], nor[4], nor[4]);

        mesh->addTriangle(pos[4], pos[0], pos[3], tex[0], tex[1], tex[2], nor[5], nor[5], nor[5]);
        mesh->addTriangle(pos[4], pos[3], pos[7], tex[0], tex[2], tex[3], nor[5], nor[5], nor[5]);
    }
    else {
        mesh->addTriangle(pos[4], pos[5], pos[6], tex[0], tex[1], tex[2], nor[0], nor[0], nor[0]);
        mesh->addTriangle(pos[4], pos[6], pos[7], tex[0], tex[2], tex[3], nor[0], nor[0], nor[0]);

        mesh->addTriangle(pos[1], pos[0], pos[3], tex[0], tex[1], tex[2], nor[1], nor[1], nor[1]);
        mesh->addTriangle(pos[1], pos[3], pos[2], tex[0], tex[2], tex[3], nor[1], nor[1], nor[1]);

        mesh->addTriangle(pos[3], pos[7], pos[6], tex[0], tex[1], tex[2], nor[2], nor[2], nor[2]);
        mesh->addTriangle(pos[3], pos[6], pos[2], tex[0], tex[2], tex[3], nor[2], nor[2], nor[2]);

        mesh->addTriangle(pos[1], pos[5], pos[4], tex[0], tex[1], tex[2], nor[3], nor[3], nor[3]);
        mesh->addTriangle(pos[1], pos[4], pos[0], tex[0], tex[2], tex[3], nor[3], nor[3], nor[3]);

        mesh->addTriangle(pos[0], pos[4], pos[7], tex[0], tex[1], tex[2], nor[4], nor[4], nor[4]);
        mesh->addTriangle(pos[0], pos[7], pos[3], tex[0], tex[2], tex[3], nor[4], nor[4], nor[4]);

        mesh->addTriangle(pos[5], pos[1], pos[2], tex[0], tex[1], tex[2], nor[5], nor[5], nor[5]);
        mesh->addTriangle(pos[5], pos[2], pos[6], tex[0], tex[2], tex[3], nor[5], nor[5], nor[5]);
    }


    return mesh;
}


typedef float (*MathFunc)(float);

template<MathFunc _Func>
void fillValueTable(std::vector<float>* table, const int count, float angle)
{
    table->reserve(count);

    const float angleStep = angle / (count - 1);

    for (int i = 0; i < count; ++i) {
        (*table).push_back(_Func(angleStep * i));
    }
}


Mesh* MeshFactory::allocSphere(ResourceStorage& storage, float radius, unsigned int cPlane) //FIXME: refactor
{
    const int sliceCount = static_cast<int>(Math::round(sqrt(static_cast<float>(cPlane))));
    const int stackCount = sliceCount;

    std::vector<float> sliceSinTable;
    std::vector<float> sliceCosTable;
    std::vector<float> stackSinTable;
    std::vector<float> stackCosTable;

    fillValueTable<std::sin>(&sliceSinTable, sliceCount + 1, Math::TWO_PI);
    fillValueTable<std::cos>(&sliceCosTable, sliceCount + 1, Math::TWO_PI);
    fillValueTable<std::sin>(&stackSinTable, stackCount + 1, Math::PI);
    fillValueTable<std::cos>(&stackCosTable, stackCount + 1, Math::PI);

    std::vector<Vec3> pos;
    pos.reserve((stackCount - 1) * sliceCount + 2);

    std::vector<Vec3> nor;
    nor.reserve((stackCount - 1) * sliceCount + 2);

    pos.push_back(Vec3(0, 0, radius));
    nor.push_back(Vec3(0, 0, 1));

    for (int i = 1; i < stackCount; ++i) {
        for (int j = 0; j < sliceCount; ++j) {
            float x = sliceCosTable[j] * stackSinTable[i];
            float y = -sliceSinTable[j] * stackSinTable[i];
            float z = stackCosTable[i];

            pos.push_back(Vec3(x * radius, y * radius, z * radius));
            nor.push_back(Vec3(x, y, z));
        }
    }

    pos.push_back(Vec3(0, 0, -radius));
    nor.push_back(Vec3(0, 0, -1));

    const float F_SLICE_COUNT = float(sliceCount);
    const float F_STACK_COUNT = float(stackCount);
    const float STEP = 1 / F_SLICE_COUNT;
    const float STEP_2 = 1 / F_STACK_COUNT;

    Mesh* mesh = new Mesh(storage);

    for (int i = 1; i < sliceCount; ++i) {
        mesh->addTriangle(
            pos[0],
            pos[i + 1],
            pos[i],

            Vec2(STEP / 2 + (i - 1) * STEP, 0.0f),
            Vec2(i * STEP, STEP_2),
            Vec2((i - 1) * STEP, STEP_2),

            nor[0],
            nor[i + 1],
            nor[i]
        );
    }

    mesh->addTriangle(
        pos[0],
        pos[1],
        pos[sliceCount],

        Vec2(1.0f - STEP, 0.0f),
        Vec2(1.0f, STEP_2),
        Vec2(1.0f - STEP, STEP_2),

        nor[0],
        nor[1],
        nor[sliceCount]
    );

    for (int i = 0; i < stackCount - 2; ++i) {

        for (int j = 0; j < sliceCount - 1; ++j) {        
            mesh->addTriangle(
                pos[1 + i * sliceCount + j],
                pos[1 + (i + 1) * sliceCount + j + 1],
                pos[1 + (i + 1) * sliceCount + j],

                Vec2(j * STEP, i * STEP_2),
                Vec2(j * STEP + STEP, (i + 1) * STEP_2),
                Vec2(j * STEP, (i + 1) * STEP_2),

                nor[1 + i * sliceCount + j],
                nor[1 + (i + 1) * sliceCount + j + 1],
                nor[1 + (i + 1) * sliceCount + j]
            );

            mesh->addTriangle(
                pos[1 + i * sliceCount + j],
                pos[1 + 1 + i * sliceCount + j],
                pos[1 + (i + 1) * sliceCount + j + 1],

                Vec2(j * STEP, i * STEP_2),
                Vec2(j * STEP + STEP, i * STEP_2),
                Vec2(j * STEP + STEP, (i + 1) * STEP_2),

                nor[1 + i * sliceCount + j],
                nor[1 + 1 + i * sliceCount + j],
                nor[1 + (i + 1) * sliceCount + j + 1]
            );
        }

        mesh->addTriangle(
            pos[1 + (i * sliceCount) + (sliceCount - 1)],
            pos[1 + (i + 1) * sliceCount],
            pos[1 + (i + 1) * sliceCount + (sliceCount - 1)],

            Vec2((sliceCount - 1) * STEP, i * STEP_2),
            Vec2(1.0f, (i + 1) * STEP_2),
            Vec2((sliceCount - 1) * STEP, (i + 1) * STEP_2),

            nor[1 + (i * sliceCount) + (sliceCount - 1)],
            nor[1 + (i + 1) * sliceCount],
            nor[1 + (i + 1) * sliceCount + (sliceCount - 1)]
        );

        mesh->addTriangle(
            pos[1 + (i * sliceCount) + (sliceCount - 1)],
            pos[1 + i * sliceCount],
            pos[1 + (i + 1) * sliceCount],

            Vec2((sliceCount - 1) * STEP, i * STEP_2),
            Vec2(1.0f, i * STEP_2),
            Vec2(1.0f, (i + 1) * STEP_2),

            nor[1 + (i * sliceCount) + (sliceCount - 1)],
            nor[1 + i * sliceCount],
            nor[1 + (i + 1) * sliceCount]
        );
    }

    const std::vector<Vec3>::size_type lastPositionIndex = pos.size() - 1;

    for (int i = 1; i < sliceCount; ++i) {
        mesh->addTriangle(
            pos[lastPositionIndex],
            pos[lastPositionIndex - 1 - sliceCount + i],
            pos[lastPositionIndex - sliceCount + i],

            Vec2(STEP / 2 + (i - 1) * STEP, 1.0f),
            Vec2((i - 1) * STEP, 1.0f - STEP_2),
            Vec2(i * STEP, 1.0f - STEP_2),

            nor[lastPositionIndex],
            nor[lastPositionIndex - 1 - sliceCount + i],
            nor[lastPositionIndex - sliceCount + i]
        );
    }

    mesh->addTriangle(
        pos[lastPositionIndex],
        pos[lastPositionIndex - 1],
        pos[lastPositionIndex - sliceCount],

        Vec2(1.0f - STEP / 2, 1.0f),
        Vec2(1.0f - STEP, 1.0f - STEP_2),
        Vec2(1.0f, 1.0f - STEP_2),

        nor[lastPositionIndex],
        nor[lastPositionIndex - 1],
        nor[lastPositionIndex - sliceCount]
    );

    return mesh;
}


}//namespace engine
