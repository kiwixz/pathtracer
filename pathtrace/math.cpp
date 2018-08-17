#include "math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

namespace pathtrace::math {
    glm::dmat4 euler_rotation(const glm::dvec3& rotation)
    {
        return glm::yawPitchRoll(-rotation.x,
                                 rotation.y,
                                 -rotation.z);
    }

    glm::dmat4 transform(const glm::dvec3& translation, const glm::dvec3& rotation, const glm::dvec3& scale)
    {
        return glm::translate(translation) * euler_rotation(rotation) * glm::scale(scale);
    }
}  // namespace pathtrace::math
