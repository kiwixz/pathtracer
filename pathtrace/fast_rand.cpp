#include "pathtrace/fast_rand.h"
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <array>
#include <cmath>
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
        // c++ forbids type punning, so we cant play efficiently smart with ieee754 binary representation
        // using division instead of std::uniform_real_distribution we lose some precision, but its much faster
        constexpr auto max = static_cast<double>(std::numeric_limits<uint64_t>::max());
        return gen_() / max;
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
        double u = (*this)(-1.0, 1.0);
        double xy = std::sqrt(1 - u * u);
        return {xy * std::cos(theta), xy * std::sin(theta), u};
    }
}  // namespace pathtracer
