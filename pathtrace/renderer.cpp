#include "pathtrace/renderer.h"

namespace pathtracer {
    Image Renderer::render(const Scene& scene)
    {
        (void)scene;
        std::vector<glm::vec3> pixels(640 * 480);
        for (int y = 0; y < 480; ++y) {
            for (int x = 0; x < 640; ++x)
                pixels[y * 640 + x] = {x / 640.f, y / 480.f, 0};
        }
        return {std::move(pixels), 640, 480};
    }
}  // namespace pathtracer
