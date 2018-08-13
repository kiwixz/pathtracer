#pragma once

#include "pathtrace/ray.h"
#include <glm/vec3.hpp>
#include <optional>

namespace pathtracer {
    struct Aabb {  /// axis-aligned bounding box
        glm::dvec3 bottom_left;
        glm::dvec3 top_right;


        Aabb(const glm::dvec3& bottom_left, const glm::dvec3& top_right);

        /// one_over_ray should be {ray.origin, 1.0 / ray.direction}
        bool intersect(const Ray& one_over_ray) const;
    };
}  // namespace pathtracer
