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
        glm::vec3 origin;
        glm::vec3 direction;

        Ray(const glm::vec3& origin, const glm::vec3& direction);
    };

    struct Shape {
    public:
        Material material;

        virtual ~Shape() = default;

        virtual std::optional<float> intersect(const Ray& ray) const = 0;
    };
}  // namespace pathtracer
