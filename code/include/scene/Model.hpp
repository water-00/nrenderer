#pragma once
#ifndef __NR_MODEL_HPP__
#define __NR_MODEL_HPP__

#include <string>
#include <vector>

#include "geometry/vec.hpp"

#include "Material.hpp"
#include "common/macros.hpp"

namespace NRenderer
{
    using namespace std;

    struct Entity {
        Handle material;
    };
    SHARE(Entity);

    struct Sphere : public Entity
    {
        Vec3 direction = {0, 0, 1};
        Vec3 position = {0, 0, 0};
        float radius = { 0 };
    };
    SHARE(Sphere);
    
    struct Triangle : public Entity
    {
        union {
            struct {
                Vec3 v1;
                Vec3 v2;
                Vec3 v3;
            };
            Vec3 v[3];
        };
        Vec3 normal;
        Triangle()
            : v1            ()
            , v2            ()
            , v3            ()
            , normal         (0, 0, 1)
        {}
        Triangle(Vec3 v1_, Vec3 v2_, Vec3 v3_) {
            v1 = v1_;
            v2 = v2_;
            v3 = v3_;
            normal = glm::cross(v1 - v2, v1 - v3);
        }
    };
    SHARE(Triangle);

    struct Plane : public Entity
    {
        Vec3 normal = {0, 0, 1};
        Vec3 position = {};
        Vec3 u = {};
        Vec3 v = {};
    };
    SHARE(Plane);

    struct Mesh : public Entity
    {
        vector<Vec3> normals;
        vector<Vec3> positions;
        vector<Vec2> uvs;
        vector<Index> normalIndices;
        vector<Index> positionIndices;
        vector<Index> uvIndices;

        bool hasNormal() const {
            return normals.size() != 0;
        }

        bool hasUv() const {
            return uvs.size() != 0;
        }
    };
    SHARE(Mesh);

    struct Node
    {
        enum class Type
        {
            SPHERE = 0x0,
            TRIANGLE = 0X1,
            PLANE = 0X2,
            MESH = 0X3
        };
        Type type = Type::SPHERE;
        Index entity; // 是第几个实体(plane/mesh/triangle/sphere)
        Index model; // 属于哪个模型
    };
    SHARE(Node);

    struct Model {
        vector<Index> nodes;
        Vec3 translation = {0, 0, 0};
        Vec3 scale = {1, 1, 1};
    };
    SHARE(Model);
}

#endif