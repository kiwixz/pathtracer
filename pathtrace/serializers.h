#pragma once

#include "pathtrace/scene.h"
#include "pathtrace/shape.h"
#include <glm/vec3.hpp>
#include <nlohmann/json.hpp>

namespace nlohmann {
    template <glm::length_t L, typename T, glm::qualifier Q>
    struct adl_serializer<glm::vec<L, T, Q>> {
        using ValueType = glm::vec<L, T, Q>;
        static void to_json(json& j, const ValueType& value);
        static void from_json(const json& j, ValueType& value);
    };

    template <>
    struct adl_serializer<pathtracer::Material> {
        using ValueType = pathtracer::Material;
        static void to_json(json& j, const ValueType& value);
        static void from_json(const json& j, ValueType& value);
    };

    template <>
    struct adl_serializer<pathtracer::Scene> {
        using ValueType = pathtracer::Scene;
        static void to_json(json& j, const ValueType& value);
        static void from_json(const json& j, ValueType& value);
    };
}  // namespace nlohmann

#include "pathtrace/serializers.inl"
