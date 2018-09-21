#include "serializers.h"
#include <pathtrace/shapes/mesh.h>
#include <pathtrace/shapes/plane.h>
#include <pathtrace/shapes/sphere.h>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

using namespace pathtrace;

namespace {
    class StaticData {
    public:
        using ShapeLoader = std::function<std::unique_ptr<Shape>(const nlohmann::json&)>;
        using ShapeSaver = std::function<void(const Shape&, nlohmann::json&)>;

        static const StaticData& get();

        Material::Reflection reflection(const std::string& reflection) const;
        const std::string& reflection(Material::Reflection reflection) const;
        const ShapeLoader& shape_loader(const std::string& type) const;
        const ShapeSaver& shape_saver(const std::string& type) const;

        template <typename T>
        const std::string& shape_type() const;
        const std::string& shape_type(const Shape& shape) const;

    private:
        StaticData() = default;

        std::unordered_map<std::string, Material::Reflection> map_reflections = {
                {"diffuse", Material::Reflection::diffuse},
                {"specular", Material::Reflection::specular},
                {"refractive", Material::Reflection::refractive},
        };

        std::unordered_map<std::type_index, std::string> map_shape_types = {
                {typeid(shapes::Sphere), "sphere"},
                {typeid(shapes::Plane), "plane"},
                {typeid(shapes::Mesh), "mesh"},
        };

        std::unordered_map<std::string, ShapeLoader> map_shape_loaders = {
                {shape_type<shapes::Sphere>(), [](const nlohmann::json& j) {
                     auto shape = std::make_unique<shapes::Sphere>();
                     shape->position = j.at("position");
                     shape->radius = j.at("radius");
                     return shape;
                 }},
                {shape_type<shapes::Plane>(), [](const nlohmann::json& j) {
                     auto shape = std::make_unique<shapes::Plane>();
                     shape->position = j.at("position");
                     shape->rotation = glm::radians(j.at("rotation").get<glm::dvec3>());
                     return shape;
                 }},
                {shape_type<shapes::Mesh>(), [](const nlohmann::json& j) {
                     auto shape = std::make_unique<shapes::Mesh>();
                     shape->position = j.at("position");
                     shape->rotation = glm::radians(j.at("rotation").get<glm::dvec3>());
                     shape->scale = j.at("scale");
                     shape->load_obj(j.at("file"));
                     return shape;
                 }},
        };

        std::unordered_map<std::string, ShapeSaver> map_shape_savers = {
                {shape_type<shapes::Sphere>(), [](const Shape& shape_, nlohmann::json& j) {
                     auto& shape = static_cast<const shapes::Sphere&>(shape_);
                     j["position"] = shape.position;
                     j["radius"] = shape.radius;
                 }},
                {shape_type<shapes::Plane>(), [](const Shape& shape_, nlohmann::json& j) {
                     auto& shape = static_cast<const shapes::Plane&>(shape_);
                     j["position"] = shape.position;
                     j["rotation"] = glm::degrees(shape.rotation);
                 }},
                {shape_type<shapes::Mesh>(), [](const Shape& shape_, nlohmann::json& j) {
                     auto& shape = static_cast<const shapes::Mesh&>(shape_);
                     j["position"] = shape.position;
                     j["rotation"] = glm::degrees(shape.rotation);
                     j["scale"] = shape.scale;
                     j["file"] = "unknown";  // TODO should we maybe save filename or save triangles ?
                 }},
        };
    };


    const StaticData& StaticData::get()
    {
        static const StaticData static_data;
        return static_data;
    }

    Material::Reflection StaticData::reflection(const std::string& reflection) const
    {
        return map_reflections.at(reflection);
    }
    const std::string& StaticData::reflection(Material::Reflection reflection) const
    {
        auto it = std::find_if(map_reflections.begin(), map_reflections.end(),
                               [&](const std::pair<std::string, Material::Reflection>& pair) {
                                   return pair.second == reflection;
                               });
        if (it == map_reflections.end())
            throw std::runtime_error{"invalid material reflection"};
        return it->first;
    }

    const StaticData::ShapeLoader& StaticData::shape_loader(const std::string& type) const
    {
        return map_shape_loaders.at(type);
    }

    const StaticData::ShapeSaver& StaticData::shape_saver(const std::string& type) const
    {
        return map_shape_savers.at(type);
    }

    template <typename T>
    const std::string& StaticData::shape_type() const
    {
        return map_shape_types.at(typeid(T));
    }
    const std::string& StaticData::shape_type(const Shape& shape) const
    {
        return map_shape_types.at(typeid(shape));
    }
}  // namespace


namespace nlohmann {
    void adl_serializer<Material>::to_json(json& j, const ValueType& value)
    {
        j["reflection"] = StaticData::get().reflection(value.reflection);
        j["color"] = value.color;
        j["emission"] = value.emission;
    }

    void adl_serializer<Material>::from_json(const json& j, ValueType& value)
    {
        value.reflection = StaticData::get().reflection(j.at("reflection").get<std::string>());
        value.color = j.at("color");
        value.emission = j.at("emission");
    }


    void adl_serializer<Scene>::to_json(json& j, const ValueType& value)
    {
        const StaticData& static_data = StaticData::get();

        nlohmann::json& j_settings = j["settings"];
        j_settings["width"] = value.settings.width;
        j_settings["height"] = value.settings.height;
        j_settings["samples"] = value.settings.samples;
        j_settings["min_bounces"] = value.settings.min_bounces;
        j_settings["max_bounces"] = value.settings.max_bounces;
        j_settings["background_color"] = value.settings.background_color;

        nlohmann::json& j_camera = j["camera"];
        j_camera["position"] = value.camera.position;
        j_camera["rotation"] = glm::degrees(value.camera.rotation);
        j_camera["scale"] = value.camera.scale;
        j_camera["field_of_view"] = glm::degrees(value.camera.field_of_view);

        nlohmann::json& j_shapes = j["shapes"];
        for (const std::unique_ptr<Shape>& shape : value.shapes) {
            const std::string& type = static_data.shape_type(*shape);
            nlohmann::json j_shape;
            static_data.shape_saver(type)(*shape, j_shape);
            j_shape["type"] = type;
            j_shape["material"] = shape->material;
            j_shapes.emplace_back(std::move(j_shape));
        }
    }

    void adl_serializer<Scene>::from_json(const json& j, ValueType& value)
    {
        nlohmann::json j_settings = j.at("settings");
        value.settings.width = j_settings.at("width");
        value.settings.height = j_settings.at("height");
        value.settings.samples = j_settings.at("samples");
        value.settings.min_bounces = j_settings.at("min_bounces");
        value.settings.max_bounces = j_settings.at("max_bounces");
        value.settings.background_color = j_settings.at("background_color");

        nlohmann::json j_camera = j.at("camera");
        value.camera.position = j_camera.at("position");
        value.camera.rotation = glm::radians(j_camera.at("rotation").get<glm::dvec3>());
        value.camera.scale = j_camera.at("scale");
        value.camera.field_of_view = glm::radians(j_camera.at("field_of_view").get<double>());

        nlohmann::json j_shapes = j.at("shapes");
        for (const nlohmann::json& j_shape : j_shapes) {
            std::unique_ptr<Shape> new_shape = StaticData::get().shape_loader(j_shape.at("type"))(j_shape);
            new_shape->material = j_shape.at("material");
            value.shapes.emplace_back(std::move(new_shape));
        }
    }
}  // namespace nlohmann
