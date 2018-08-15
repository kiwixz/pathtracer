#pragma once

#include <glm/vec3.hpp>
#include <vector>

namespace pathtracer {
    using Color = glm::dvec3;

    class Image {
    public:
        Image() = default;
        Image(std::vector<Color>&& pixels, int width, int height);

        const std::vector<Color>& pixels() const;
        int width() const;
        int height() const;

        void clamp(double min = 0, double max = 1);

        template <typename T>
        std::vector<T> convert(double dithering = 1) const;

    private:
        std::vector<Color> pixels_;
        int width_ = 0;
        int height_ = 0;
    };
}  // namespace pathtracer

#include "pathtrace/image.inl"
