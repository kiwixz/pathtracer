#pragma once

#include "pathtrace/shape.h"
#include <array>
#include <vector>

namespace pathtrace::shapes {
    struct Triangle {
        std::array<glm::dvec3, 3> vertices;
        glm::dvec3 normal;

        Triangle() = default;
        Intersection intersect(const Ray& ray, const Shape& shape) const;

        /// may compute inverted normal
        void guess_normal();
    };

    struct Mesh : public Shape {
        glm::dvec3 position;
        glm::dvec3 rotation;
        glm::dvec3 scale;
        std::vector<Triangle> triangles;

        // baked
        std::vector<Triangle> transformed_triangles;

        void bake() override;
        Intersection intersect(const Ray& ray) const override;

        void load_obj(const std::string& path);
    };
}  // namespace pathtrace::shapes
