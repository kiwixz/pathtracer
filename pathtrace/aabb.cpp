#include "aabb.h"
#include <glm/geometric.hpp>
#include <algorithm>

namespace pathtrace {
    Aabb::Aabb(const glm::dvec3& bottom_left, const glm::dvec3& top_right) :
        bottom_left{bottom_left}, top_right{top_right}
    {}

    std::optional<double> Aabb::intersect(const Ray& one_over_ray) const
    {
        const glm::dvec3& min = bottom_left;
        const glm::dvec3& max = top_right;

        double tmin = ((one_over_ray.direction.x < 0 ? max : min).x - one_over_ray.origin.x) * one_over_ray.direction.x;
        double tmax = ((one_over_ray.direction.x < 0 ? min : max).x - one_over_ray.origin.x) * one_over_ray.direction.x;
        double tymin = ((one_over_ray.direction.y < 0 ? max : min).y - one_over_ray.origin.y) * one_over_ray.direction.y;
        double tymax = ((one_over_ray.direction.y < 0 ? min : max).y - one_over_ray.origin.y) * one_over_ray.direction.y;

        if (tmin > tymax || tymin > tmax)
            return {};

        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;

        double tzmin = ((one_over_ray.direction.z < 0 ? max : min).z - one_over_ray.origin.z) * one_over_ray.direction.z;
        double tzmax = ((one_over_ray.direction.z < 0 ? min : max).z - one_over_ray.origin.z) * one_over_ray.direction.z;

        if (tmin > tzmax || tzmin > tmax)
            return {};

        return std::max(tmin, tzmin);
    }

    bool Aabb::contains(const glm::dvec3& point) const
    {
        return point.x >= bottom_left.x && point.x <= top_right.x
               && point.y >= bottom_left.y && point.y <= top_right.y
               && point.z >= bottom_left.z && point.z <= top_right.z;
    }
}  // namespace pathtrace
