#include "pathtrace/shapes/mesh.h"
#include "mesh.h"
#include "pathtrace/math.h"
#include <glm/geometric.hpp>

namespace pathtrace::shapes {
    std::optional<double> Triangle::intersect(const Ray& ray) const
    {
        return std::optional<double>();
    }

    void Mesh::bake()
    {}

    Intersection Mesh::intersect(const Ray& ray) const
    {}
}  // namespace pathtrace::shapes
