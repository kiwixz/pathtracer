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
    void adl_serializer<Material::Reflection>::to_json(json& j, const ValueType& value)
    {
        auto it = std::find_if(map_reflections.begin(), map_reflections.end(),
                               [&](const std::pair<std::string, ValueType>& pair) {
                                   return pair.second == value;
                               });
        if (it == map_reflections.end())
            throw std::runtime_error{"invalid material reflection"};
        j = it->first;
    }

    void adl_serializer<Material::Reflection>::from_json(const json& j, ValueType& value)
    {
        value = map_reflections.at(j);
    }
}  // namespace nlohmann
