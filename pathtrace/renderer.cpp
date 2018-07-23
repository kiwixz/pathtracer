#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "pathtrace/renderer.h"
#include <glm/mat4x4.hpp>

namespace pathtracer {
    Image Renderer::render(const Scene& scene)
    {
        std::vector<glm::vec3> pixels(scene.settings.width * scene.settings.height);

        glm::mat4 projection = glm::orientate4(scene.camera.position);
        projection[0][3] = scene.camera.position.x;
        projection[1][3] = scene.camera.position.y;
        projection[2][3] = scene.camera.position.z;

        for (int y = 0; y < scene.settings.height; ++y) {
            for (int x = 0; x < scene.settings.width; ++x) {
                Ray ray{scene.camera.position,
                        glm::normalize(
                                glm::vec4{(x / static_cast<float>(scene.settings.width) - .5f)
                                                  * scene.settings.width / scene.settings.height,
                                        y / static_cast<float>(scene.settings.height) - .5f,
                                        scene.camera.focal, 0}
                                * projection)};

                const Shape* shape = nullptr;
                float distance = std::numeric_limits<float>::infinity();
                for (const std::unique_ptr<Shape>& s : scene.shapes) {
                    std::optional<float> dist = s->intersect(ray);
                    if (dist && dist < distance) {
                        distance = dist.value();
                        shape = s.get();
                    }
                }

                if (shape)
                    pixels[y * scene.settings.width + x] = shape->material.color;
                else
                    pixels[y * scene.settings.width + x] = {50, 50, 50};
            }
        }
        return {std::move(pixels), scene.settings.width, scene.settings.height};
    }
}  // namespace pathtracer
