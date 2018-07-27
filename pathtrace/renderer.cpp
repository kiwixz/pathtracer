#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "pathtrace/renderer.h"
#include <glm/mat4x4.hpp>

namespace pathtracer {
    namespace {
        constexpr double pi = 3.1415926535897932;
        constexpr Color background_color{0, 0, 0};

        float random(float a, float b)
        {
            float rnd = rand() / (float)RAND_MAX;
            return rnd * (b - a) + a;
        }
        float random(float b = 1)
        {
            return random(0, b);
        }

        Color radiance(const Ray& ray, const Scene& scene, int depth = 0)
        {
            const Shape* shape_ptr = nullptr;
            float distance = std::numeric_limits<float>::infinity();
            for (const std::unique_ptr<Shape>& s : scene.shapes) {
                std::optional<float> new_distance = s->intersect(ray);
                if (new_distance && new_distance < distance) {
                    distance = new_distance.value();
                    shape_ptr = s.get();
                }
            }

            if (!shape_ptr)
                return background_color;

            const Shape& shape = *shape_ptr;

            if (depth > 2)  // TODO russian roulette with color
                return shape.material.emission;

            glm::vec3 intersection = ray.origin + ray.direction * distance;
            glm::vec3 normal = shape.normal(intersection);
            glm::vec3 oriented_normal = glm::dot(normal, ray.direction) > 0 ? -normal : normal;

            switch (shape.material.reflection) {
            case Material::Reflection::diffuse:  // TODO WIP
                float r1 = random(2 * pi), r2 = random(), r2s = std::sqrt(r2);
                glm::vec3 u = glm::normalize(glm::cross(
                        std::abs(oriented_normal.x) > .1 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0),
                        oriented_normal));
                glm::vec3 v = glm::cross(oriented_normal, u);
                glm::vec3 d = glm::normalize(u * std::cos(r1) * r2s + v * std::sin(r1) * r2s
                                             + oriented_normal * std::sqrt(1 - r2));
                return shape.material.emission
                       + shape.material.color * radiance(Ray(intersection, d), scene, depth + 1);
            }

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
                Color& color = pixels[y * scene.settings.width + x];
                color = {0, 0, 0};
                for (int i = 0; i < scene.settings.samples; ++i) {
                    glm::vec3 point_on_screen{(x / static_cast<float>(scene.settings.width) - .5f)
                                                      * scene.settings.width
                                                      / scene.settings.height,
                            .5f - y / static_cast<float>(scene.settings.height),
                            scene.camera.focal};
                    glm::vec3 eye_to_screen = projection * glm::vec4{point_on_screen, 1};
                    Ray ray{scene.camera.position, glm::normalize(eye_to_screen)};

                    color += radiance(ray, scene) / static_cast<float>(scene.settings.samples);
                }
            }
        }
        return {std::move(pixels), scene.settings.width, scene.settings.height};
    }
}  // namespace pathtracer
