#pragma once

#include <pathtrace/shape.h>
#include <array>

namespace pathtrace::shapes {
    struct Triangle : public Shape {
        std::array<glm::dvec3, 3> vertices;
        glm::dvec3 normal;

        // baked
        glm::dvec3 v0_to_v1;
        glm::dvec3 v0_to_v2;


        Triangle() = default;
        void bake() override;
        Intersection intersect(const Ray& ray) const override;

        /// may compute inverted normal
        void guess_normal();
    };
}  // namespace pathtrace::shapes
