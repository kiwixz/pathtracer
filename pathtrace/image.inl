#pragma once

#include "pathtrace/image.h"
#include <algorithm>
#include <cmath>

namespace pathtracer {
    template <typename T>
    std::vector<T> Image::convert(float dithering) const
    {
        std::vector<float> work_pixels(pixels_.size() * 3);
        for (unsigned i = 0; i < pixels_.size(); ++i)
            for (unsigned c = 0; c < 3; ++c)
                work_pixels[i * 3 + c] = std::clamp(pixels_[i][c], 0.f, 1.f);

        auto get_work = [&](int x, int y, int c) -> float& {
            return work_pixels[(y * width_ + x) * 3 + c];
        };

        constexpr int max = std::numeric_limits<T>::max();

        // floyd-steinberg dithering
        for (int y = 0; y < height_; ++y) {
            for (int x = 0; x < width_; ++x) {
                for (int c = 0; c < 3; ++c) {
                    float old_pix = get_work(x, y, c);
                    float new_pix = std::round(old_pix * max) / max;
                    get_work(x, y, c) = new_pix;
                    float quant_error = (old_pix - new_pix) * dithering;

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
        std::transform(work_pixels.begin(), work_pixels.end(), result.begin(), [&](float linear) {
            int target = static_cast<int>(std::round(linear * max));
            return static_cast<T>(std::clamp(target, 0, max));
        });
        return result;
    }
}  // namespace pathtracer
