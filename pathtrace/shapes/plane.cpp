#include "pathtrace/shapes/plane.h"
#include "pathtrace/math.h"
#include <glm/geometric.hpp>

namespace pathtrace::shapes {
    void Plane::bake()
    {
        up = math::euler_rotation(rotation) * glm::dvec4{0, 1, 0, 1};
    }

    Intersection Plane::intersect(const Ray& ray) const
    {
        constexpr double epsilon = 1e-12;

        double denominator = glm::dot(ray.direction, up);
        if (!denominator)  // parallel
            return {};
        double distance = glm::dot(position - ray.origin, up) / denominator;
        if (distance < epsilon)  // is behind
            return {};
        return {this, distance, ray, up};
    }
}  // namespace pathtrace::shapes
