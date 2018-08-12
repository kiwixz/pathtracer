#include "pathtrace/sphere.h"
#include <glm/geometric.hpp>

namespace pathtracer {
    Aabb::Aabb(const glm::dvec3& position, const glm::dvec3& size) :
        position{position}, size{size}
    {}
}  // namespace pathtracer
