#include "pathtrace/sphere.h"
#include <glm/geometric.hpp>

namespace pathtracer {
    std::optional<float> Sphere::intersect(const Ray& ray) const
    {
        float radius_sq = radius * radius;  // TODO precompute

        glm::vec3 vec_to_pos = position - ray.origin;
        float middle = glm::dot(vec_to_pos, ray.direction);
        float distance_middle_center_sq = glm::dot(vec_to_pos, vec_to_pos) - middle * middle;
        if (distance_middle_center_sq > radius_sq)
            return {};
        float middle_to_intersection = std::sqrt(radius_sq - distance_middle_center_sq);

        float distance = middle - middle_to_intersection;
        if (distance <= 0) {
            distance = middle + middle_to_intersection;
            if (distance <= 0)
                return {};
        }

        return distance;
    }
}  // namespace pathtracer
