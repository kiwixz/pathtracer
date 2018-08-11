#include "pathtrace/serializers.h"

using namespace pathtracer;

namespace {
    const std::map<std::string, Material::Reflection> map_reflections = {
            {"diffuse", Material::Reflection::diffuse},
            {"specular", Material::Reflection::specular},
            {"refractive", Material::Reflection::refractive},
    };
}

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
}  // namespace nlohmann
