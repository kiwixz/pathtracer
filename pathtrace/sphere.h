#pragma once

#include "pathtrace/shape.h"

namespace pathtracer {
    struct Sphere : public Shape {
    public:
        glm::vec3 position;
        float radius;

        std::optional<float> intersect(const Ray& ray) const;
    };
}  // namespace pathtracer
