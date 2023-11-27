/**
 * @file    tree_mesh_builder.cpp
 *
 * @author  FULL NAME <xlogin00@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    DATE
 **/

#include <iostream>
#include <math.h>
#include <limits>

#include "tree_mesh_builder.h"

const float sqrt_3_over_2 = sqrtf(3.f) / 2.f; 

TreeMeshBuilder::TreeMeshBuilder(unsigned gridEdgeSize)
    : BaseMeshBuilder(gridEdgeSize, "Octree")
{

}

// recursive function
unsigned TreeMeshBuilder::marchCubesRecursive(const ParametricScalarField &field,
                                              const Position newPos,
                                              const unsigned newGridSize) {

    // trivial case, the grid size is too small to be divided further
    // new evaluate build all cubes
    if (newGridSize < 2) return buildCube(newPos, field);

    // calculate the center of the current cube in field coordinate system
    // to decide whether to recursively call for smaller cubes
    const float halfSize = newGridSize / 2.f;
    // moving further in each dimension
    const Position newPosCenter = Position {
        (newPos.x + halfSize) * mGridResolution,
        (newPos.y + halfSize) * mGridResolution,
        (newPos.z + halfSize) * mGridResolution
    };

    // F(p) > l + (sqrt(3)/2)*a
    if (evaluateFieldAt(newPosCenter, field) > mIsoLevel + (sqrt_3_over_2 * newGridSize * mGridResolution)) return 0.0f;

    unsigned totalTriangles = 0;
    // #pragma omp parallel for
    for (int i = 0; i < 8; i++) {
        const Position newP = {
            newPos.x + ((i & 1) * halfSize),
            newPos.y + (((i >> 1) & 1) * halfSize),
            newPos.z + (((i >> 2) & 1) * halfSize),
        };
        
        totalTriangles += marchCubesRecursive(field, newP, halfSize);
    }
    return totalTriangles;
} 

// recursion entry
unsigned TreeMeshBuilder::marchCubes(const ParametricScalarField &field) {
    const Position initP = Position {0.f,0.f,0.f};
    return marchCubesRecursive(field, initP, mGridSize);
}

float TreeMeshBuilder::evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field) {
    // NOTE: This method is called from "buildCube(...)"!

    // 1. Store pointer to and number of 3D points in the field
    //    (to avoid "data()" and "size()" call in the loop).
    const Vec3_t<float> *pPoints = field.getPoints().data();
    const unsigned count = unsigned(field.getPoints().size());

    float value = std::numeric_limits<float>::max();

    // 2. Find minimum square distance from points "pos" to any point in the
    //    field.
    // #pragma omp simd reduction(min: value) simdlen(16)
    for(unsigned i = 0; i < count; ++i) {
        float distanceSquared  = (pos.x - pPoints[i].x) * (pos.x - pPoints[i].x);
        distanceSquared       += (pos.y - pPoints[i].y) * (pos.y - pPoints[i].y);
        distanceSquared       += (pos.z - pPoints[i].z) * (pos.z - pPoints[i].z);

        // Comparing squares instead of real distance to avoid unnecessary
        // "sqrt"s in the loop.
        if (value > distanceSquared) value = distanceSquared;
    }

    // 3. Finally take square root of the minimal square distance to get the real distance
    return sqrt(value);
}

void TreeMeshBuilder::emitTriangle(const BaseMeshBuilder::Triangle_t &triangle) {
    // #pragma omp critical
    mTriangles.push_back(triangle);
}