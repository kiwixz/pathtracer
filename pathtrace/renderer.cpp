#include "pathtrace/renderer.h"

namespace pathtracer {
    Image Renderer::render(const Scene& scene)
    {
        (void)scene;
        return {std::vector<glm::vec3>(640 * 480, {255, 0, 0}), 640, 480};
    }
}  // namespace pathtracer
