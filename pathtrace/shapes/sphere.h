#pragma once

#include <pathtrace/shape.h>

namespace pathtrace::shapes {
    struct Sphere : public Shape {
        glm::dvec3 position;
        double radius;

        // baked
        double radius_sq;


        Sphere() = default;
        void bake() override;
        Intersection intersect(const Ray& ray) const override;
    };
}  // namespace pathtrace::shapes
