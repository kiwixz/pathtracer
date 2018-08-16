#pragma once

#include "pathtrace/shape.h"

namespace pathtrace::shapes {
    struct Plane : public Shape {
        glm::dvec3 position;
        glm::dvec3 rotation;

        // baked
        glm::dvec3 up;


        Plane() = default;
        void bake() override;
        Intersection intersect(const Ray& ray) const override;
    };
}  // namespace pathtrace::shapes
