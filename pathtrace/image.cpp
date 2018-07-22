#include "pathtrace/image.h"

namespace pathtracer {
    Image::Image(std::vector<glm::vec3>&& pixels, int width, int height) :
        pixels_{std::move(pixels)}, width_{width}, height_{height}
    {}

    const std::vector<glm::vec3>& Image::pixels() const
    {
        return pixels_;
    }
    int Image::width() const
    {
        return width_;
    }
    int Image::height() const
    {
        return height_;
    }

    std::vector<uint8_t> Image::convert_to_24_bits() const
    {
        return std::vector<uint8_t>(width_ * height_ * 3, 255);
    }

    std::vector<uint16_t> Image::convert_to_48_bits() const
    {
        return std::vector<uint16_t>(width_ * height_ * 3, 32767);
    }
}  // namespace pathtracer
