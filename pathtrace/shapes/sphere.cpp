#include "pathtrace/shapes/sphere.h"
#include <glm/geometric.hpp>

namespace pathtrace::shapes {
    void Sphere::bake()
    {
        radius_sq = radius * radius;
    }

    Intersection Sphere::intersect(const Ray& ray) const
    {
        constexpr double epsilon = 1e-9;

        glm::dvec3 vec_to_pos = position - ray.origin;
        double middle = glm::dot(vec_to_pos, ray.direction);
        double distance_middle_center_sq = glm::dot(vec_to_pos, vec_to_pos) - middle * middle;
        if (distance_middle_center_sq > radius_sq)  // no intersection
            return {};
        double middle_to_intersection = std::sqrt(radius_sq - distance_middle_center_sq);

        double distance = middle - middle_to_intersection;
        if (distance < epsilon) {  // this point is behind, check the other
            distance = middle + middle_to_intersection;
            if (distance < epsilon)  // behind too
                return {};
        }

        glm::dvec3 point = ray.origin + ray.direction * distance;
        return {this, distance, point, glm::normalize(point - position)};
    }
}  // namespace pathtrace::shapes
