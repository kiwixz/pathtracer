#include "mesh.h"
#include <pathtrace/math.h>

// #define TINYOBJLOADER_USE_DOUBLE  // should work but make tinyobjloader go crazy
#include <tiny_obj_loader.h>

namespace pathtrace::shapes {
    void Mesh::bake()
    {
        glm::dmat4 transformation = math::transform(position, rotation, scale);
        glm::dmat4 pseudo_transformation = glm::transpose(glm::inverse(transformation));  // do not deform normals etc

        std::vector<Triangle> transformed_triangles;
        transformed_triangles.reserve(triangles.size());
        std::transform(triangles.begin(), triangles.end(), std::back_inserter(transformed_triangles),
                       [&](const Triangle& triangle) {
                           Triangle new_triangle;
                           new_triangle.material = material;
                           new_triangle.vertices = {transformation * glm::dvec4{triangle.vertices[0], 1},
                                                    transformation * glm::dvec4{triangle.vertices[1], 1},
                                                    transformation * glm::dvec4{triangle.vertices[2], 1}};
                           new_triangle.normal = glm::normalize(glm::dvec3{
                                   pseudo_transformation * glm::dvec4{triangle.normal, 1}});

                           new_triangle.bake();
                           return new_triangle;
                       });

        octree = {transformed_triangles};
    }

    Intersection Mesh::intersect(const Ray& ray) const
    {
        return octree.intersect(ray);
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
            for (uint8_t nr_vertices[[maybe_unused]] : shape.mesh.num_face_vertices) {  // iterate over faces
                assert(nr_vertices == 3);
                Triangle triangle;
                triangle.normal = glm::dvec3{0};  // TODO GLM_FORCE_CTOR_INIT
                int nr_normals = 0;
                for (int i = 0; i < 3; ++i) {
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + i];
                    triangle.vertices[i] = {attrib.vertices[3 * idx.vertex_index + 0],
                                            attrib.vertices[3 * idx.vertex_index + 1],
                                            attrib.vertices[3 * idx.vertex_index + 2]};
                    if (idx.normal_index >= 0) {
                        triangle.normal += glm::dvec3{attrib.normals[3 * idx.normal_index + 0],
                                                      attrib.normals[3 * idx.normal_index + 1],
                                                      attrib.normals[3 * idx.normal_index + 2]};
                        ++nr_normals;
                    }
                }
                index_offset += 3;

                if (nr_normals)
                    triangle.normal = glm::normalize(triangle.normal / static_cast<double>(nr_normals));
                else
                    triangle.guess_normal();
                triangles.emplace_back(std::move(triangle));
            }
        }
    }
}  // namespace pathtrace::shapes
