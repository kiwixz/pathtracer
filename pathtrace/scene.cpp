#include "pathtrace/scene.h"
#include "pathtrace/sphere.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace pathtracer {
    namespace {
        glm::dvec3 parse_vec3(const nlohmann::json& j)
        {
            if (j.size() != 3)
                throw std::runtime_error{"could not parse vec3: size mismatch"};
            return {j[0], j[1], j[2]};
        }


        const std::map<std::string, std::function<std::unique_ptr<Shape>(const nlohmann::json&)>>
                loaders_shape = {{"sphere", [](const nlohmann::json& j) {
                                      auto shape = std::make_unique<Sphere>();
                                      shape->position = parse_vec3(j.at("position"));
                                      shape->radius = j.at("radius");
                                      return shape;
                                  }}};

        const std::map<std::string, Material::Reflection> map_reflections = {
                {"diffuse", Material::Reflection::diffuse},
                {"specular", Material::Reflection::specular},
                {"refractive", Material::Reflection::refractive},
        };


        Material parse_material(const nlohmann::json& j)
        {
            Material mat;
            mat.reflection = map_reflections.at(j.at("reflection"));
            mat.color = parse_vec3(j.at("color"));
            mat.emission = parse_vec3(j.at("emission"));
            return mat;
        }
    }  // namespace

    void Scene::load_from_file(const std::string& path)
    {
        std::ifstream ifs(path);
        if (!ifs)
            throw std::runtime_error{"could not open scene file"};
        nlohmann::json j = nlohmann::json::parse(ifs);

        nlohmann::json j_settings = j.at("settings");
        settings.width = j_settings.at("width");
        settings.height = j_settings.at("height");
        settings.samples = j_settings.at("samples");
        settings.max_bounces = j_settings.at("max_bounces");

        nlohmann::json j_camera = j.at("camera");
        camera.position = parse_vec3(j_camera.at("position"));
        camera.rotation = parse_vec3(j_camera.at("rotation"));
        camera.focal = j_camera.at("focal");

        for (const nlohmann::json& j_shape : j.at("shapes")) {
            std::unique_ptr<Shape> new_shape = loaders_shape.at(j_shape.at("type"))(j_shape);
            new_shape->material = parse_material(j_shape.at("material"));
            shapes.emplace_back(std::move(new_shape));
        }
    }

    void Scene::save_to_file(const std::string& path) const
    {
        (void)path;
        // TODO
    }
}  // namespace pathtracer
