#include "ray.h"

namespace pathtrace {
    Ray::Ray(const glm::dvec3& origin, const glm::dvec3& direction) :
        origin{origin}, direction{direction}
    {}
}  // namespace pathtrace
