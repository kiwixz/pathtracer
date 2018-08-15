#pragma once

#include <glm/vec3.hpp>

namespace pathtrace {
    struct Ray {
        glm::dvec3 origin;
        glm::dvec3 direction;

        Ray() = default;
        Ray(const glm::dvec3& origin, const glm::dvec3& direction);
    };
}  // namespace pathtrace
