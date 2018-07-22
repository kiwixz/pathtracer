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

        std::vector<uint8_t> convert_to_24_bits() const;
        std::vector<uint16_t> convert_to_48_bits() const;

    private:
        std::vector<glm::vec3> pixels_;
        int width_ = 0;
        int height_ = 0;
    };
}  // namespace pathtracer
