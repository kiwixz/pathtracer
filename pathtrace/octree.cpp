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

    void OctreeNode::add_triangle(const shapes::Triangle& triangle)
    {
        if (!children_ && triangles_.size() <= min_triangles_intersect_aabb - 1) {
            if (triangles_.size() < min_triangles_intersect_aabb - 1)
                triangles_.emplace_back(triangle);
            else {  // evaluate triangles to make sure we dont have to add them in
                std::vector<shapes::Triangle> old_triangles = std::move(triangles_);
                triangles_.clear();

                // initialize children
                children_ = std::make_unique<std::array<OctreeNode, 8>>();
                glm::dvec3 half = (aabb_.top_right - aabb_.bottom_left) / 2.0;
                glm::dvec3 middle = aabb_.bottom_left + half;
                for (int i = 0; i < 8; ++i) {
                    glm::dvec3 bottom_left{i % 4 < 2 ? aabb_.bottom_left.x : middle.x,
                                           i % 2 ? aabb_.bottom_left.y : middle.y,
                                           i < 4 ? aabb_.bottom_left.z : middle.z};
                    (*children_)[i] = {{bottom_left, bottom_left + half}};
                }

                for (const shapes::Triangle& old_triangle : old_triangles)
                    add_triangle_placed(old_triangle);
                add_triangle_placed(triangle);
            }
        }
        else
            add_triangle_placed(triangle);
    }

    Intersection OctreeNode::intersect(const Ray& ray, double max_distance) const
    {
        if (triangles_.size() >= min_triangles_intersect_aabb || children_) {
            std::optional<double> distance = aabb_.intersect(ray);
            if (!distance || distance > max_distance)
                return {};
        }

        Intersection intersection;

        for (const shapes::Triangle& triangle : triangles_) {
            Intersection new_intersection = triangle.intersect(ray);
            if (new_intersection && new_intersection.distance() < intersection.distance())
                intersection = std::move(new_intersection);
        }

        if (children_)
            for (const OctreeNode& child : *children_) {
                Intersection new_intersection = child.intersect(ray, intersection.distance());
                if (new_intersection && new_intersection.distance() < intersection.distance())
                    intersection = std::move(new_intersection);
            }

        return intersection;
    }

    void OctreeNode::add_triangle_placed(const shapes::Triangle& triangle)
    {
        // TODO triangle-aabb intersection?

        for (OctreeNode& child : *children_)
            if (child.aabb_.contains(triangle.vertices[0])
                && child.aabb_.contains(triangle.vertices[1])
                && child.aabb_.contains(triangle.vertices[2])) {
                child.add_triangle(triangle);
                return;
            }

        triangles_.emplace_back(triangle);
    }


    Octree::Octree(const std::vector<shapes::Triangle>& triangles) :
        OctreeNode{infer_aabb(triangles)}
    {
        for (const shapes::Triangle& triangle : triangles)
            add_triangle(triangle);
    }
}  // namespace pathtrace
