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

    const std::unordered_map<std::type_index, std::string> map_shape_types = {
            {typeid(Sphere), "sphere"},
    };
    template <typename T>
    const std::string& shape_type()
    {
        return map_shape_types.at(typeid(T));
    }
    const std::string& shape_type(const Shape& shape)
    {
        return map_shape_types.at(typeid(shape));
    }

    using ShapeLoader = std::function<std::unique_ptr<Shape>(const nlohmann::json&)>;
    const std::unordered_map<std::string, ShapeLoader> map_shape_loaders = {
            {shape_type<Sphere>(), [](const nlohmann::json& j) {
                 auto shape = std::make_unique<Sphere>();
                 shape->position = j.at("position");
                 shape->radius = j.at("radius");
                 return shape;
             }},
    };

    using ShapeSaver = std::function<void(const Shape&, nlohmann::json&)>;
    const std::unordered_map<std::string, ShapeSaver> map_shape_savers = {
            {shape_type<Sphere>(), [](const Shape& shape_, nlohmann::json& j) {
                 auto& shape = reinterpret_cast<const Sphere&>(shape_);
                 j["position"] = shape.position;
                 j["radius"] = shape.radius;
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
        nlohmann::json& j_settings = j["settings"];
        j_settings["width"] = value.settings.width;
        j_settings["height"] = value.settings.height;
        j_settings["samples"] = value.settings.samples;
        j_settings["max_bounces"] = value.settings.max_bounces;
        j_settings["background_color"] = value.settings.background_color;

        nlohmann::json& j_camera = j["camera"];
        j_camera["position"] = value.camera.position;
        j_camera["rotation"] = glm::degrees(value.camera.rotation);
        j_camera["field_of_view"] = glm::degrees(value.camera.field_of_view);

        nlohmann::json& j_shapes = j["shapes"];
        for (const std::unique_ptr<Shape>& shape : value.shapes) {
            const std::string type = shape_type(*shape);
            nlohmann::json j;
            map_shape_savers.at(type)(*shape, j);
            j["type"] = type;
            j["material"] = shape->material;
            j_shapes.emplace_back(std::move(j));
        }
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
