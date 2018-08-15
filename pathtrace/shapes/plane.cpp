#include "pathtrace/shapes/plane.h"
#include "pathtrace/math.h"
#include <glm/geometric.hpp>

namespace pathtrace::shapes {
    void Plane::bake()
    {
        up = math::euler_rotation(rotation) * glm::dvec4{0, 1, 0, 1};
    }

    std::optional<double> Plane::intersect(const Ray& ray) const
    {
        double denominator = glm::dot(ray.direction, up);
        if (!denominator)  // parallel
            return {};
        double distance = glm::dot(position - ray.origin, up) / denominator;
        if (distance <= 0)  // is behind
            return {};
        return distance;
    }

    glm::dvec3 Plane::normal(const glm::dvec3& intersection) const
    {
        return up;
    }
}  // namespace pathtrace::shapes
