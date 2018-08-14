#pragma once

#include "pathtrace/shape.h"

namespace pathtracer {
    struct Sphere : public Shape {
        glm::dvec3 position;
        double radius;

        Aabb aabb() const override;
        std::optional<double> intersect(const Ray& ray) const override;
        glm::dvec3 normal(const glm::dvec3& intersection) const override;
    };
}  // namespace pathtracer
