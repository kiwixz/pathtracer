#include "pathtrace/aabb.h"
#include <glm/geometric.hpp>

namespace pathtracer {
    Aabb::Aabb(const glm::dvec3& position, const glm::dvec3& size) :
        position{position}, size{size}
    {}

    std::optional<double> Aabb::intersect(const Ray& ray) const
    {
        return {1};  // TODO (aabb intersect) actually implement
    }
}  // namespace pathtracer
