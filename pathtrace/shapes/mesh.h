#pragma once

#include "pathtrace/shape.h"
#include <array>
#include <vector>

namespace pathtrace::shapes {
    struct Triangle {
        std::array<glm::dvec3, 3> vertices;

        std::optional<double> intersect(const Ray& ray) const;
    };

    struct Mesh : public Shape {
        glm::dvec3 position;
        glm::dvec3 rotation;
        double scale;
        std::vector<Triangle> triangles;

        // baked
        ;

        void bake() override;
        std::optional<double> intersect(const Ray& ray) const override;
        glm::dvec3 normal(const glm::dvec3& intersection) const override;
    };
}  // namespace pathtrace::shapes
