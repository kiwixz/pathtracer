#include "pathtrace/scene.h"
#include "pathtrace/serializers.h"
#include "pathtrace/sphere.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace pathtracer {
    namespace {
        using LoaderShape = std::function<std::unique_ptr<Shape>(const nlohmann::json&)>;
        const std::map<std::string, LoaderShape> loaders_shape = {
                {"sphere", [](const nlohmann::json& j) {
                     auto shape = std::make_unique<Sphere>();
                     shape->position = j.at("position");
                     shape->radius = j.at("radius");
                     return shape;
                 }},
        };

        const std::map<std::string, Material::Reflection> map_reflections = {
                {"diffuse", Material::Reflection::diffuse},
                {"specular", Material::Reflection::specular},
                {"refractive", Material::Reflection::refractive},
        };


        Material parse_material(const nlohmann::json& j)
        {
            Material mat;
            mat.reflection = map_reflections.at(j.at("reflection"));
            mat.color = j.at("color");
            mat.emission = j.at("emission");
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
        settings.background_color = j_settings.at("background_color");

        nlohmann::json j_camera = j.at("camera");
        camera.position = j_camera.at("position");
        camera.rotation = glm::radians(j_camera.at("rotation").get<glm::dvec3>());
        camera.fov = glm::radians(j_camera.at("fov").get<double>());

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
