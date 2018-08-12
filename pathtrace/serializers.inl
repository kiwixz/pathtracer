#pragma once

#include "pathtrace/serializers.h"

namespace nlohmann {
    template <glm::length_t L, typename T, glm::qualifier Q>
    void adl_serializer<glm::vec<L, T, Q>>::to_json(json& j, const ValueType& value)
    {
        j.clear();
        for (glm::length_t i = 0; i < L; ++i)
            j.emplace_back(value[i]);
    }

    template <glm::length_t L, typename T, glm::qualifier Q>
    void adl_serializer<glm::vec<L, T, Q>>::from_json(const json& j, ValueType& value)
    {
        for (glm::length_t i = 0; i < L; ++i)
            value[i] = j[i];
    }
}  // namespace nlohmann
