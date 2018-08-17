#include "octree.h"
#include <glm/common.hpp>

namespace pathtrace {
    namespace {
        constexpr int min_triangles_intersect_aabb = 2;


        Aabb infer_aabb(const std::vector<shapes::Triangle>& triangles)
        {
            assert(!triangles.empty());
            Aabb aabb{{std::numeric_limits<double>::infinity(),
                       std::numeric_limits<double>::infinity(),
                       std::numeric_limits<double>::infinity()},
                      {-std::numeric_limits<double>::infinity(),
                       -std::numeric_limits<double>::infinity(),
                       -std::numeric_limits<double>::infinity()}};
            for (const shapes::Triangle& triangle : triangles)
                for (const glm::dvec3& vertice : triangle.vertices) {
                    aabb.bottom_left = glm::min(aabb.bottom_left, vertice);
                    aabb.top_right = glm::max(aabb.top_right, vertice);
                }
            return aabb;
        }
    }  // namespace


    OctreeNode::OctreeNode(const Aabb& aabb) :
        aabb_{aabb}
    {}

    bool OctreeNode::empty() const
    {
        return std::holds_alternative<Nothing>(elements_);
    }

    void OctreeNode::add_triangle(const shapes::Triangle& triangle)
    {
        if (std::holds_alternative<Nothing>(elements_))
            elements_ = std::vector<shapes::Triangle>{triangle};
        else if (std::holds_alternative<Triangles>(elements_)) {
            std::vector<shapes::Triangle>& triangles = std::get<Triangles>(elements_);
            if (triangles.size() < min_triangles_intersect_aabb - 1) {
                triangles.emplace_back(triangle);
                return;
            }

            std::vector<shapes::Triangle> old_triangles = std::move(triangles);

            elements_ = std::make_unique<std::array<OctreeNode, 8>>();
            std::array<OctreeNode, 8>& children = *std::get<Children>(elements_);
            glm::dvec3 half = (aabb_.top_right - aabb_.bottom_left) / 2.0;
            glm::dvec3 middle = aabb_.bottom_left + half;
            for (int i = 0; i < 8; ++i) {
                glm::dvec3 bottom_left{i % 4 < 2 ? aabb_.bottom_left.x : middle.x,
                                       i % 2 ? aabb_.bottom_left.y : middle.y,
                                       i < 4 ? aabb_.bottom_left.z : middle.z};
                children[i] = {{bottom_left, bottom_left + half}};
            }

            for (const shapes::Triangle& old_triangle : old_triangles)
                add_triangle(old_triangle);
            add_triangle(triangle);
        }
        else if (std::holds_alternative<Children>(elements_)) {
            std::array<OctreeNode, 8>& children = *std::get<Children>(elements_);

            // TODO highly uneffective, replace by triangle-aabb intersection

            for (OctreeNode& child : children)
                if (child.aabb_.contains(triangle.vertices[0])
                    && child.aabb_.contains(triangle.vertices[1])
                    && child.aabb_.contains(triangle.vertices[2])) {
                    child.add_triangle(triangle);
                    return;
                }

            for (OctreeNode& child : children)
                child.add_triangle(triangle);
        }
    }


    Octree::Octree(const std::vector<shapes::Triangle>& triangles) :
        OctreeNode{infer_aabb(triangles)}
    {
        for (const shapes::Triangle& triangle : triangles)
            add_triangle(triangle);
    }
}  // namespace pathtrace
