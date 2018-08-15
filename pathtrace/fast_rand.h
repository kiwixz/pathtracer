#pragma once

#include <glm/vec3.hpp>
#include <random>

namespace pathtrace {
    class FastRand {
    public:
        FastRand();

        /// returns a number in [0; 1)
        double operator()();

        /// returns a number in [0; max)
        double operator()(double max);

        /// returns a number in [min; max)
        double operator()(double min, double max);

        /// returns a normalized vector
        glm::dvec3 sphere();

    private:
        std::mt19937_64 gen_;
    };
}  // namespace pathtrace
