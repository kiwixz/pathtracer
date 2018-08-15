#include "pathtrace/image.h"
#include <glm/common.hpp>

namespace pathtrace {
    Image::Image(std::vector<Color>&& pixels, int width, int height) :
        pixels_{std::move(pixels)}, width_{width}, height_{height}
    {}

    const std::vector<Color>& Image::pixels() const
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

    void Image::clamp(double min, double max)
    {
        for (Color& pix : pixels_)
            glm::clamp(pix, min, max);
    }
}  // namespace pathtrace
