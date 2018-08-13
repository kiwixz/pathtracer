#include "pathtrace/aabb.h"
#include <glm/geometric.hpp>

namespace pathtracer {
    Aabb::Aabb(const glm::dvec3& position, const glm::dvec3& size) :
        position{position}, size{size}
    {}

    bool Aabb::intersect(const Ray& one_over_ray) const
    {
        const glm::dvec3& min = position;
        glm::dvec3 max = position + size;

        double tmin, tmax, tymin, tymax, tzmin, tzmax;

        tmin = ((one_over_ray.direction.x < 0 ? max : min).x - one_over_ray.origin.x) * one_over_ray.direction.x;
        tmax = ((one_over_ray.direction.x < 0 ? min : max).x - one_over_ray.origin.x) * one_over_ray.direction.x;
        tymin = ((one_over_ray.direction.y < 0 ? max : min).y - one_over_ray.origin.y) * one_over_ray.direction.y;
        tymax = ((one_over_ray.direction.y < 0 ? min : max).y - one_over_ray.origin.y) * one_over_ray.direction.y;

        if (tmin > tymax || tymin > tmax)
            return false;

        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;

        tzmin = ((one_over_ray.direction.z < 0 ? max : min).z - one_over_ray.origin.z) * one_over_ray.direction.z;
        tzmax = ((one_over_ray.direction.z < 0 ? min : max).z - one_over_ray.origin.z) * one_over_ray.direction.z;

        return !(tmin > tzmax || tzmin > tmax);
    }
}  // namespace pathtracer
