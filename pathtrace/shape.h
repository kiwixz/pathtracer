#pragma once

#include <pathtrace/aabb.h>
#include <pathtrace/image.h>
#include <pathtrace/ray.h>
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
        Material(Reflection _reflection, const Color& _emission, const Color& _color);
    };


    class Intersection {
    public:
        Intersection() = default;
        Intersection(const Shape* shape, double distance, const Ray& ray, const glm::dvec3& normal);
        Intersection(const Shape* shape, double distance, const glm::dvec3& point, const glm::dvec3& normal);
        explicit operator bool() const;

        const Shape* shape() const;
        double distance() const;
        const glm::dvec3& point() const;
        const glm::dvec3& normal() const;

    private:
        const Shape* shape_ = nullptr;
        double distance_ = std::numeric_limits<double>::infinity();
        glm::dvec3 point_;
        glm::dvec3 normal_;
    };


    struct Shape {
        Material material;

        virtual ~Shape() = default;
        virtual void bake() = 0;
        virtual Intersection intersect(const Ray& ray) const = 0;
    };
}  // namespace pathtrace
