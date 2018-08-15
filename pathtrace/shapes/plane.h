#pragma once

#include "pathtrace/shape.h"

namespace pathtrace::shapes {
    struct Plane : public Shape {
        glm::dvec3 position;
        glm::dvec3 rotation;

        // baked
        glm::dvec3 up;

        void bake() override;
        std::optional<double> intersect(const Ray& ray) const override;
        glm::dvec3 normal(const glm::dvec3& intersection) const override;
    };
}  // namespace pathtrace::shapes
