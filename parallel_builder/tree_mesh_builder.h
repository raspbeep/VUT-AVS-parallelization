/**
 * @file    tree_mesh_builder.h
 *
 * @author  FULL NAME <xkrato61@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    DATE
 **/

#ifndef TREE_MESH_BUILDER_H
#define TREE_MESH_BUILDER_H

#include "base_mesh_builder.h"

typedef Vec3_t<float> Position;

class TreeMeshBuilder : public BaseMeshBuilder
{
public:
    TreeMeshBuilder(unsigned gridEdgeSize);

protected:
    unsigned marchCubes(const ParametricScalarField &field);
    unsigned marchCubesRecursive(const ParametricScalarField &field,
                                                  const Position newPos,
                                                  const unsigned newGridSize);
    float evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field);
    void emitTriangle(const Triangle_t &triangle);
    const Triangle_t *getTrianglesArray() const { return mTriangles.data(); }

    std::vector <Triangle_t> mTriangles; ///< Temporary array of triangles

};

#endif // TREE_MESH_BUILDER_H
