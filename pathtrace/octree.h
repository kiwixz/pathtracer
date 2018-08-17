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

        bool empty() const;
        void add_triangle(const shapes::Triangle& triangle);

    private:
        // for elements
        using Nothing = std::monostate;
        using Triangles = std::vector<shapes::Triangle>;
        using Children = std::unique_ptr<std::array<OctreeNode, 8>>;

        Aabb aabb_;
        std::variant<Nothing, Children, Triangles> elements_;
    };


    class Octree : public OctreeNode {
    public:
        Octree() = default;
        Octree(const std::vector<shapes::Triangle>& triangles);
    };
}  // namespace pathtrace
