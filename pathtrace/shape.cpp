#include "pathtrace/shape.h"

namespace pathtracer {
    Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) :
        origin(origin), direction(direction)
    {}
}  // namespace pathtracer
