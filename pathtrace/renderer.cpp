#include "pathtrace/renderer.h"

namespace pathtracer {
    Image Renderer::render(const Scene& scene)
    {
        std::vector<glm::vec3> pixels(scene.settings.width * scene.settings.height);
        for (int y = 0; y < scene.settings.height; ++y) {
            for (int x = 0; x < scene.settings.width; ++x)
                pixels[y * scene.settings.width + x] = {
                        x / static_cast<float>(scene.settings.width),
                        y / static_cast<float>(scene.settings.height), 0};
        }
        return {std::move(pixels), scene.settings.width, scene.settings.height};
    }
}  // namespace pathtracer
