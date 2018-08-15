#include "pathtrace/math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace pathtrace::math {
    glm::dmat4 euler_rotation(const glm::dvec3& rotation)
    {
        return glm::yawPitchRoll(rotation.x, rotation.y, rotation.z);
    }

    glm::dmat4 transform(const glm::dvec3& translation, const glm::dvec3& rotation, const glm::dvec3& scale)
    {
        glm::dmat4 r = euler_rotation(rotation);

        r[0][0] *= scale.x;
        r[1][1] *= scale.y;
        r[2][2] *= scale.z;

        r[3][0] = translation.x;
        r[3][1] = translation.y;
        r[3][2] = translation.z;

        return r;
    }
}  // namespace pathtrace::math
