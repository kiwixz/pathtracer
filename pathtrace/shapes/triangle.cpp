#include "triangle.h"
#include <glm/geometric.hpp>

namespace pathtrace::shapes {
    void Triangle::bake()
    {
        v0_to_v2 = vertices[2] - vertices[0];
        v0_to_v1 = vertices[1] - vertices[0];
    }

    Intersection Triangle::intersect(const Ray& ray, const Shape& shape) const
    {
        constexpr double epsilon = 1e-12;

        // Möller-Trumbore algorithm

        glm::dvec3 pvec = glm::cross(ray.direction, v0_to_v2);

        double determinant = glm::dot(pvec, v0_to_v1);
        if (determinant == 0)  // parallel
            return {};
        double inverse_determinant = 1 / determinant;

        glm::dvec3 v0_to_origin = ray.origin - vertices[0];
        double u = glm::dot(v0_to_origin, pvec) * inverse_determinant;
        if (u < 0 || u > 1)
            return {};

        glm::dvec3 qvec = glm::cross(v0_to_origin, v0_to_v1);
        double v = glm::dot(ray.direction, qvec) * inverse_determinant;
        if (v < 0 || u + v > 1)
            return {};

        double distance = glm::dot(v0_to_v2, qvec) * inverse_determinant;
        if (distance < epsilon)  // is behind
            return {};

        return {&shape, distance, ray, normal};
    }

    void Triangle::guess_normal()
    {
        normal = glm::normalize(glm::cross(vertices[1] - vertices[0],
                                           vertices[2] - vertices[0]));
    }
}  // namespace pathtrace::shapes
