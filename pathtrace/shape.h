#pragma once

#include "pathtrace/aabb.h"
#include "pathtrace/image.h"
#include "pathtrace/ray.h"
#include <glm/vec3.hpp>
#include <optional>

namespace pathtrace {
    struct Shape;

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

    struct Intersection {
        const Shape* shape = nullptr;
        double distance = std::numeric_limits<double>::infinity();
        glm::dvec3 point;
        glm::dvec3 normal;

        Intersection() = default;
        Intersection(const Shape* shape, double distance, const Ray& ray, const glm::dvec3& normal);
        Intersection(const Shape* shape, double distance, const glm::dvec3& point, const glm::dvec3& normal);
        explicit operator bool() const;
    };

    struct Shape {
        Material material;

        virtual ~Shape() = default;

        virtual void bake() = 0;
        virtual Intersection intersect(const Ray& ray) const = 0;
    };
}  // namespace pathtrace
