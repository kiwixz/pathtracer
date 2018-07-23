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
}  // namespace pathtracer
