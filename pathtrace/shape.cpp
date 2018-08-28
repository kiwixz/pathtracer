#include "shape.h"

namespace pathtrace {
    Material::Material(Reflection _reflection, const Color& _emission, const Color& _color) :
        reflection{_reflection}, emission{_emission}, color{_color}
    {}


    Intersection::Intersection(const Shape* shape, double distance, const Ray& ray, const glm::dvec3& normal) :
        Intersection{shape, distance, ray.origin() + ray.direction() * distance, normal}
    {}

    Intersection::Intersection(const Shape* shape, double distance, const glm::dvec3& point, const glm::dvec3& normal) :
        shape_{shape}, distance_{distance}, point_{point}, normal_{normal}
    {}

    Intersection::operator bool() const
    {
        return shape_;
    }

    const Shape* Intersection::shape() const
    {
        return shape_;
    }
    double Intersection::distance() const
    {
        return distance_;
    }
    const glm::dvec3& Intersection::point() const
    {
        return point_;
    }
    const glm::dvec3& Intersection::normal() const
    {
        return normal_;
    }
}  // namespace pathtrace
