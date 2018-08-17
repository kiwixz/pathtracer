#include "shape.h"

namespace pathtrace {
    Material::Material(Reflection reflection, const Color& emission, const Color& color) :
        reflection{reflection}, emission{emission}, color{color}
    {}


    Intersection::Intersection(const Shape* shape, double distance, const Ray& ray, const glm::dvec3& normal) :
        Intersection{shape, distance, ray.origin() + ray.direction() * distance, normal}
    {}

    Intersection::Intersection(const Shape* shape, double distance, const glm::dvec3& point, const glm::dvec3& normal) :
        shape{shape}, distance{distance}, point{point}, normal{normal}
    {}

    Intersection::operator bool() const
    {
        return shape;
    }
}  // namespace pathtrace
