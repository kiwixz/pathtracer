#pragma once

#include <glm/vec3.hpp>
#include <optional>

namespace pathtracer {
    struct Material {
        enum class Reflection {
            diffuse,    // default
            specular,   // mirror
            refractive  // glass
        };

        Reflection reflection = Reflection::diffuse;
        glm::vec3 emission;
        glm::vec3 color;
    };

    struct Ray {
        glm::dvec3 origin;
        glm::dvec3 direction;

        Ray(const glm::dvec3& origin, const glm::dvec3& direction);
    };

    struct Shape {
    public:
        Material material;

        virtual ~Shape() = default;

        virtual std::optional<double> intersect(const Ray& ray) const = 0;
        virtual glm::dvec3 normal(const glm::dvec3& intersection) const = 0;
    };
}  // namespace pathtracer
