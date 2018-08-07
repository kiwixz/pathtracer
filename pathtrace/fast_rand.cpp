#include "pathtrace/fast_rand.h"
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <array>
#include <functional>

namespace pathtracer {
    FastRand::FastRand()
    {
        // properly seed full state of generator
        std::array<uint64_t, decltype(gen_)::state_size> seed_data;
        std::random_device source;
        std::generate(seed_data.begin(), seed_data.end(), std::ref(source));
        std::seed_seq seed_seq(seed_data.begin(), seed_data.end());
        gen_.seed(seed_seq);
    }

    double FastRand::operator()()
    {
        return std::ldexp(gen_(), -64);
    }
    double FastRand::operator()(double max)
    {
        return (*this)() * max;
    }
    double FastRand::operator()(double min, double max)
    {
        return (*this)() * (max - min) + min;
    }

    glm::dvec3 FastRand::sphere()
    {
        double theta = (*this)(glm::two_pi<double>());
        double u = (*this)(-1., 1.);
        double xy = std::sqrt(1 - u * u);
        return {xy * std::cos(theta), xy * std::sin(theta), u};
    }
}  // namespace pathtracer
