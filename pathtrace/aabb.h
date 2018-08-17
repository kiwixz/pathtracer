#pragma once

#include <pathtrace/ray.h>
#include <glm/vec3.hpp>
#include <optional>

namespace pathtrace {
    struct Aabb {  /// axis-aligned bounding box
        glm::dvec3 bottom_left;
        glm::dvec3 top_right;


        Aabb() = default;
        Aabb(const glm::dvec3& bottom_left, const glm::dvec3& top_right);

        /// one_over_ray should be {ray.origin, 1.0 / ray.direction}
        std::optional<double> intersect(const Ray& one_over_ray) const;

        /// check if the point is inside
        bool contains(const glm::dvec3& point) const;
    };
}  // namespace pathtrace
