#include "pathtrace/ray.h"

namespace pathtracer {
    Ray::Ray(const glm::dvec3& origin, const glm::dvec3& direction) :
        origin{origin}, direction{direction}
    {}
}  // namespace pathtracer
