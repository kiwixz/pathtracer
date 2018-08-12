#pragma once

#include <glm/vec3.hpp>

namespace pathtracer {
    struct Aabb {  /// axis-aligned bounding box
        glm::dvec3 position;
        glm::dvec3 size;

        Aabb(const glm::dvec3& position, const glm::dvec3& size);
    };
}  // namespace pathtracer
