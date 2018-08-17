#include "ray.h"
#include <glm/geometric.hpp>

namespace pathtrace {
    Ray::Ray(const glm::dvec3& origin, const glm::dvec3& direction) :
        origin_{origin}, direction_{direction}
    {}

    const glm::dvec3& Ray::origin() const
    {
        return origin_;
    }
    const glm::dvec3& Ray::direction() const
    {
        return direction_;
    }
    const glm::dvec3& Ray::inverse_direction() const
    {
        if (!inversed)
            inverse_direction_ = glm::normalize(1.0 / direction_);
        return inverse_direction_;
    }
}  // namespace pathtrace
