#include "aabb.h"
#include <glm/geometric.hpp>
#include <algorithm>

namespace pathtrace {
    Aabb::Aabb(const glm::dvec3& bottom_left, const glm::dvec3& top_right) :
        bottom_left{bottom_left}, top_right{top_right}
    {}

    std::optional<double> Aabb::intersect(const Ray& ray) const
    {
        const glm::dvec3& min = bottom_left;
        const glm::dvec3& max = top_right;

        double tmin = ((ray.inverse_direction().x < 0 ? max : min).x - ray.origin().x) * ray.inverse_direction().x;
        double tmax = ((ray.inverse_direction().x < 0 ? min : max).x - ray.origin().x) * ray.inverse_direction().x;
        double tymin = ((ray.inverse_direction().y < 0 ? max : min).y - ray.origin().y) * ray.inverse_direction().y;
        double tymax = ((ray.inverse_direction().y < 0 ? min : max).y - ray.origin().y) * ray.inverse_direction().y;

        if (tmin > tymax || tymin > tmax)
            return {};

        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;

        double tzmin = ((ray.inverse_direction().z < 0 ? max : min).z - ray.origin().z) * ray.inverse_direction().z;
        double tzmax = ((ray.inverse_direction().z < 0 ? min : max).z - ray.origin().z) * ray.inverse_direction().z;

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
