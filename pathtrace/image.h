#pragma once

#include <glm/vec3.hpp>
#include <vector>

namespace pathtracer {
    class Image {
    public:
        Image() = default;
        Image(std::vector<glm::vec3>&& pixels, int width, int height);

        const std::vector<glm::vec3>& pixels() const;
        int width() const;
        int height() const;

        template <typename T>
        std::vector<T> convert(float dithering = 1) const;

    private:
        std::vector<glm::vec3> pixels_;
        int width_ = 0;
        int height_ = 0;
    };
}  // namespace pathtracer

#include "pathtrace/image.tcc"
