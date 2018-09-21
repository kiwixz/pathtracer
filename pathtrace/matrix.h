#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace pathtrace::matrix {
    /// rotation with euler angles: yaw, pitch, roll
    glm::dmat4 euler_rotation(const glm::dvec3& rotation);

    /// translation * rotation * scale
    glm::dmat4 transform(const glm::dvec3& translation, const glm::dvec3& rotation, const glm::dvec3& scale);
}  // namespace pathtrace::matrix
