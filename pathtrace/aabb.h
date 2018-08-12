#pragma once

#include "pathtrace/ray.h"
#include <glm/vec3.hpp>
#include <optional>

namespace pathtracer {
    struct Aabb {  /// axis-aligned bounding box
        glm::dvec3 position;
        glm::dvec3 size;


        Aabb(const glm::dvec3& position, const glm::dvec3& size);

        std::optional<double> intersect(const Ray& ray) const;
    };
}  // namespace pathtracer
