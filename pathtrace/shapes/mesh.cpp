#include "pathtrace/shapes/mesh.h"
#include "mesh.h"
#include "pathtrace/math.h"
#include <glm/geometric.hpp>

namespace pathtrace::shapes {
    Triangle::Triangle(std::array<glm::dvec3, 3>&& vertices) :
        vertices{std::move(vertices)}
    {}

    Intersection Triangle::intersect(const Ray& ray) const
    {
        return {};
    }


    void Mesh::bake()
    {
        glm::dmat4 transformation = math::transform(position, rotation, scale);

        transformed_triangles.clear();
        transformed_triangles.reserve(triangles.size());
        std::transform(triangles.begin(), triangles.end(), std::back_inserter(transformed_triangles),
                       [&](const Triangle& triangle) {
                           return Triangle{{transformation * glm::dvec4{triangle.vertices[0], 1},
                                            transformation * glm::dvec4{triangle.vertices[1], 1},
                                            transformation * glm::dvec4{triangle.vertices[2], 1}}};
                       });
    }

    Intersection Mesh::intersect(const Ray& ray) const
    {
        Intersection intersection;
        for (const Triangle& triangle : transformed_triangles) {
            Intersection new_intersection = triangle.intersect(ray);
            if (new_intersection && new_intersection.distance < intersection.distance) {
                intersection = std::move(new_intersection);
            }
        }
        return intersection;
    }
}  // namespace pathtrace::shapes
