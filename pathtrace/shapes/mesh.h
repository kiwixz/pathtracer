#pragma once

#include <pathtrace/octree.h>
#include <pathtrace/shape.h>
#include <pathtrace/shapes/triangle.h>
#include <vector>

namespace pathtrace::shapes {
    struct Mesh : public Shape {
        glm::dvec3 position;
        glm::dvec3 rotation;
        glm::dvec3 scale;
        std::vector<Triangle> triangles;

        // baked
        Octree octree;


        Mesh() = default;
        void bake() override;
        Intersection intersect(const Ray& ray) const override;

        void load_obj(const std::string& path);
    };
}  // namespace pathtrace::shapes
