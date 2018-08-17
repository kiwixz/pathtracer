#pragma once

#include "image.h"
#include <algorithm>
#include <cmath>

namespace pathtrace {
    template <typename T>
    std::vector<T> Image::convert(double dithering) const
    {
        std::vector<double> work_pixels(pixels_.size() * 3);
        for (unsigned i = 0; i < pixels_.size(); ++i)
            for (unsigned c = 0; c < 3; ++c)
                work_pixels[i * 3 + c] = std::clamp(pixels_[i][c], 0.0, 1.0);

        auto get_work = [&](int x, int y, int c) -> double& {
            return work_pixels[(y * width_ + x) * 3 + c];
        };

        constexpr T max = std::numeric_limits<T>::max();

        // floyd-steinberg dithering
        for (int y = 0; y < height_; ++y) {
            for (int x = 0; x < width_; ++x) {
                for (int c = 0; c < 3; ++c) {
                    double old_pix = get_work(x, y, c);
                    double new_pix = std::round(old_pix * max) / max;
                    get_work(x, y, c) = new_pix;
                    double quant_error = (old_pix - new_pix) * dithering;

                    if (x < width_ - 1)
                        get_work(x + 1, y, c) += quant_error * 7 / 16;
                    if (y < height_ - 1) {
                        if (x > 0)
                            get_work(x - 1, y + 1, c) += quant_error * 3 / 16;
                        get_work(x, y + 1, c) += quant_error * 5 / 16;
                        if (x < width_ - 1)
                            get_work(x + 1, y + 1, c) += quant_error * 1 / 16;
                    }
                }
            }
        }

        std::vector<T> result(work_pixels.size());
        std::transform(work_pixels.begin(), work_pixels.end(), result.begin(), [&](double linear) {
            T target = static_cast<T>(std::round(linear * max));
            return static_cast<T>(std::clamp(target, T{}, max));
        });
        return result;
    }
}  // namespace pathtrace
