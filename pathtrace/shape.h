#pragma once

#include "pathtrace/aabb.h"
#include "pathtrace/image.h"
#include "pathtrace/ray.h"
#include <glm/vec3.hpp>
#include <optional>

namespace pathtrace {
    struct Material {
        enum class Reflection {
            diffuse,     // default
            specular,    // mirror
            refractive,  // glass
        };

        Reflection reflection = Reflection::diffuse;
        Color emission;
        Color color;

        Material() = default;
        Material(Reflection reflection, const Color& emission, const Color& color);
    };

    struct Shape {
        Material material;

        virtual ~Shape() = default;

        virtual void bake() = 0;
        virtual std::optional<double> intersect(const Ray& ray) const = 0;
        virtual glm::dvec3 normal(const glm::dvec3& intersection) const = 0;
    };
}  // namespace pathtrace
