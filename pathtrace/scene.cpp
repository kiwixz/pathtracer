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

        Material parse_material(const nlohmann::json& j)
        {
            Material mat;
            mat.reflection = j.at("reflection");
            mat.color = j.at("color");
            mat.emission = j.at("emission");
            return mat;
        }
    }  // namespace


    void Scene::load_from_json(const std::string& source)
    {
        nlohmann::json j = nlohmann::json::parse(source);

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

        nlohmann::json j_shapes = j.at("shapes");
        for (const nlohmann::json& j_shape : j_shapes) {
            std::unique_ptr<Shape> new_shape = loaders_shape.at(j_shape.at("type"))(j_shape);
            new_shape->material = parse_material(j_shape.at("material"));
            shapes.emplace_back(std::move(new_shape));
        }
    }

    void Scene::load_from_file(const std::string& path)
    {
        std::ifstream ifs{path};
        if (!ifs)
            throw std::runtime_error{"could not open scene file"};

        ifs.seekg(0, std::ios::end);
        size_t scene_size = ifs.tellg();
        ifs.seekg(0);

        std::string scene_json(scene_size, '\0');
        ifs.read(scene_json.data(), scene_size);

        return load_from_json(scene_json);
    }

    std::string Scene::save_to_json(int pretty_indent) const
    {
        return {};  // TODO save_to_json
    }

    void Scene::save_to_file(const std::string& path, int pretty_indent) const
    {
        std::ofstream ofs{path};
        ofs << save_to_json(pretty_indent);
    }
}  // namespace pathtracer
