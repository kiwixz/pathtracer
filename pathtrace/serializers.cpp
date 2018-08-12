#include "pathtrace/serializers.h"
#include "pathtrace/sphere.h"
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

using namespace pathtracer;

namespace {
    const std::unordered_map<std::string, Material::Reflection> map_reflections = {
            {"diffuse", Material::Reflection::diffuse},
            {"specular", Material::Reflection::specular},
            {"refractive", Material::Reflection::refractive},
    };

    const std::unordered_map<std::type_index, std::string> map_shape_names = {
            {typeid(Sphere), "sphere"},
    };
    template <typename T>
    const std::string& shape_name()
    {
        return map_shape_names.at(typeid(T));
    }
    const std::string& shape_name(const Shape& shape)
    {
        return map_shape_names.at(typeid(shape));
    }

    using LoaderShape = std::function<std::unique_ptr<Shape>(const nlohmann::json&)>;
    const std::unordered_map<std::string, LoaderShape> map_shape_loaders = {
            {shape_name<Sphere>(), [](const nlohmann::json& j) {
                 auto shape = std::make_unique<Sphere>();
                 shape->position = j.at("position");
                 shape->radius = j.at("radius");
                 return shape;
             }},
    };
}  // namespace


namespace nlohmann {
    void adl_serializer<Material>::to_json(json& j, const ValueType& value)
    {
        auto it = std::find_if(map_reflections.begin(), map_reflections.end(),
                               [&](const std::pair<std::string, Material::Reflection>& pair) {
                                   return pair.second == value.reflection;
                               });
        if (it == map_reflections.end())
            throw std::runtime_error{"invalid material reflection"};

        j["reflection"] = it->first;
        j["color"] = value.color;
        j["emission"] = value.emission;
    }

    void adl_serializer<Material>::from_json(const json& j, ValueType& value)
    {
        value.reflection = map_reflections.at(j.at("reflection"));
        value.color = j.at("color");
        value.emission = j.at("emission");
    }


    void adl_serializer<Scene>::to_json(json& j, const ValueType& value)
    {
        // TODO (scene to_json)
    }

    void adl_serializer<Scene>::from_json(const json& j, ValueType& value)
    {
        nlohmann::json j_settings = j.at("settings");
        value.settings.width = j_settings.at("width");
        value.settings.height = j_settings.at("height");
        value.settings.samples = j_settings.at("samples");
        value.settings.max_bounces = j_settings.at("max_bounces");
        value.settings.background_color = j_settings.at("background_color");

        nlohmann::json j_camera = j.at("camera");
        value.camera.position = j_camera.at("position");
        value.camera.rotation = glm::radians(j_camera.at("rotation").get<glm::dvec3>());
        value.camera.field_of_view = glm::radians(j_camera.at("field_of_view").get<double>());

        nlohmann::json j_shapes = j.at("shapes");
        for (const nlohmann::json& j_shape : j_shapes) {
            std::unique_ptr<Shape> new_shape = map_shape_loaders.at(j_shape.at("type"))(j_shape);
            new_shape->material = j_shape.at("material");
            value.shapes.emplace_back(std::move(new_shape));
        }
    }
}  // namespace nlohmann
