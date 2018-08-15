#include "pathtrace/shapes/mesh.h"
#include "mesh.h"
#include "pathtrace/math.h"
#include <glm/geometric.hpp>

#define TINYOBJLOADER_USE_DOUBLE
#include <tiny_obj_loader.h>

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

    void Mesh::load_obj(const std::string& path)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str()))
            throw std::runtime_error{"could not load obj file: " + path + ": " + err};

        triangles.clear();
        size_t size = 0;
        for (tinyobj::shape_t& shape : shapes)
            size += shape.mesh.num_face_vertices.size();
        triangles.reserve(size);

        for (tinyobj::shape_t& shape : shapes) {
            int index_offset = 0;
            for (uint8_t& number_of_vertices : shape.mesh.num_face_vertices) {  // iterate over faces
                assert(number_of_vertices == 3);
                std::array<glm::dvec3, 3> vertices;
                for (int i = 0; i < 3; i++) {
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + i];
                    vertices[i] = {attrib.vertices[3 * idx.vertex_index + 0],
                                   attrib.vertices[3 * idx.vertex_index + 1],
                                   attrib.vertices[3 * idx.vertex_index + 2]};
                }
                index_offset += 3;
                triangles.emplace_back(std::move(vertices));
            }
        }

        printf("%llu triangles\n", triangles.size());
    }
}  // namespace pathtrace::shapes
