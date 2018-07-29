#include "pathtrace/sphere.h"
#include <glm/geometric.hpp>

namespace pathtracer {
    std::optional<double> Sphere::intersect(const Ray& ray) const
    {
        double radius_sq = radius * radius;  // TODO precompute

        glm::dvec3 vec_to_pos = position - ray.origin;
        double middle = glm::dot(vec_to_pos, ray.direction);
        double distance_middle_center_sq = glm::dot(vec_to_pos, vec_to_pos) - middle * middle;
        if (distance_middle_center_sq > radius_sq)
            return {};
        double middle_to_intersection = std::sqrt(radius_sq - distance_middle_center_sq);

        double distance = middle - middle_to_intersection;
        if (distance <= 0) {
            distance = middle + middle_to_intersection;
            if (distance <= 0)
                return {};
        }

        return distance;
    }

    glm::dvec3 Sphere::normal(const glm::dvec3& intersection) const
    {
        return glm::normalize(intersection - position);
    }
}  // namespace pathtracer