#pragma once

#include <pathtrace/aabb.h>
#include <pathtrace/shapes/triangle.h>
#include <array>
#include <memory>
#include <variant>
#include <vector>

namespace pathtrace {
    class OctreeNode {
    public:
        OctreeNode() = default;
        OctreeNode(const Aabb& aabb);

        void add_triangle(const shapes::Triangle& triangle);
        Intersection intersect(const Ray& ray, double max_distance = std::numeric_limits<double>::infinity()) const;

    private:
        Aabb aabb_;
        std::vector<shapes::Triangle> triangles_;
        std::unique_ptr<std::array<OctreeNode, 8>> children_;  // allocated only when at least one is non-empty

        void add_triangle_placed(const shapes::Triangle& triangle);
    };


    class Octree : public OctreeNode {
    public:
        Octree() = default;
        Octree(const std::vector<shapes::Triangle>& triangles);
    };
}  // namespace pathtrace
