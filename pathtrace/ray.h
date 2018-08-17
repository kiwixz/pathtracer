#pragma once

#include <glm/vec3.hpp>

namespace pathtrace {
    class Ray {
    public:
        Ray() = default;
        Ray(const glm::dvec3& origin, const glm::dvec3& direction);

        const glm::dvec3& origin() const;
        const glm::dvec3& direction() const;
        const glm::dvec3& inverse_direction() const;

    private:
        glm::dvec3 origin_;
        glm::dvec3 direction_;

        bool inversed = false;
        mutable glm::dvec3 inverse_direction_;
    };
}  // namespace pathtrace
