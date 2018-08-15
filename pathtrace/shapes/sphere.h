#pragma once

#include "pathtrace/shape.h"

namespace pathtrace {
    struct Sphere : public Shape {
        glm::dvec3 position;
        double radius;

        // baked
        double radius_sq;

        void bake() override;
        std::optional<double> intersect(const Ray& ray) const override;
        glm::dvec3 normal(const glm::dvec3& intersection) const override;
    };
}  // namespace pathtrace
