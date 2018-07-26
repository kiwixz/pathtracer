#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "pathtrace/renderer.h"
#include <glm/mat4x4.hpp>

namespace pathtracer {
    namespace {
        Color radiance(const Ray& ray, const Scene& scene)
        {
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
                return shape->material.color;
            else
                return {0, 1, 0};
        }
    }  // namespace

    Image Renderer::render(const Scene& scene)
    {
        glm::mat4 projection = glm::yawPitchRoll(
                scene.camera.rotation.x, scene.camera.rotation.y, scene.camera.rotation.z);
        projection[0][3] = scene.camera.position.x;
        projection[1][3] = scene.camera.position.y;
        projection[2][3] = scene.camera.position.z;

        std::vector<Color> pixels(scene.settings.width * scene.settings.height);
        for (int y = 0; y < scene.settings.height; ++y) {
            for (int x = 0; x < scene.settings.width; ++x) {
                glm::vec3 point_on_screen{(x / static_cast<float>(scene.settings.width) - .5f)
                                                  * scene.settings.width / scene.settings.height,
                        .5f - y / static_cast<float>(scene.settings.height), scene.camera.focal};
                glm::vec3 eye_to_screen = projection * glm::vec4{point_on_screen, 1};
                Ray ray{scene.camera.position, glm::normalize(eye_to_screen)};

                pixels[y * scene.settings.width + x] = radiance(ray, scene);
            }
        }
        return {std::move(pixels), scene.settings.width, scene.settings.height};
    }
}  // namespace pathtracer
