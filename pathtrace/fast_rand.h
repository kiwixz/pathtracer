#pragma once

#include <glm/vec3.hpp>
#include <random>

namespace pathtracer {
    class FastRand {
    public:
        FastRand();

        double operator()();            /// between 0 and 1
        double operator()(double max);  /// between 0 and max
        double operator()(double min, double max);

        glm::dvec3 sphere();                           /// normalized vector
        glm::dvec3 hemisphere(const glm::dvec3& ref);  /// normalized vector in hemisphere of ref

    private:
        std::mt19937_64 gen_;
    };
}  // namespace pathtracer
