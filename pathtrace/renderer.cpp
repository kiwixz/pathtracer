#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "pathtrace/renderer.h"
#include <glm/mat4x4.hpp>

namespace pathtracer {
    namespace {
        constexpr Color background_color{0, 0, 0};
    }  // namespace

    Image Renderer::render(const Scene& scene)
    {
        glm::dmat4 projection = glm::yawPitchRoll(
                scene.camera.rotation.x, scene.camera.rotation.y, scene.camera.rotation.z);
        projection[0][3] = scene.camera.position.x;
        projection[1][3] = scene.camera.position.y;
        projection[2][3] = scene.camera.position.z;

        double x_delta = 1.0 / scene.settings.width;
        double y_delta = 1.0 / scene.settings.height;
        std::vector<Color> pixels(scene.settings.width * scene.settings.height);
        for (int y = 0; y < scene.settings.height; ++y) {
            for (int x = 0; x < scene.settings.width; ++x) {
                Color& color = pixels[y * scene.settings.width + x];
                color = {0, 0, 0};
                glm::dvec3 pixel_on_screen{
                        (x * x_delta - .5) * scene.settings.width / scene.settings.height,
                        .5 - y * y_delta, scene.camera.focal};
                for (int i = 0; i < scene.settings.samples; ++i) {
                    glm::dvec4 point_on_screen{pixel_on_screen.x + gen_(x_delta),
                            pixel_on_screen.y + gen_(y_delta), pixel_on_screen.z, 1};
                    glm::dvec3 eye_to_screen = projection * point_on_screen;
                    Ray ray{scene.camera.position, glm::normalize(eye_to_screen)};
                    color += radiance(ray, scene) / static_cast<float>(scene.settings.samples);
                }
            }
        }
        return {std::move(pixels), scene.settings.width, scene.settings.height};
    }

    Color Renderer::radiance(const Ray& ray, const Scene& scene, int depth)
    {
        const Shape* shape_ptr = nullptr;
        double distance = std::numeric_limits<double>::infinity();
        for (const std::unique_ptr<Shape>& s : scene.shapes) {
            std::optional<double> new_distance = s->intersect(ray);
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

        glm::dvec3 intersection = ray.origin + ray.direction * (distance - 1e-9);  // TODO justify epsilon
        glm::dvec3 normal = shape.normal(intersection);
        glm::dvec3 oriented_normal = glm::dot(normal, ray.direction) > 0 ? -normal : normal;

        switch (shape.material.reflection) {
        case Material::Reflection::diffuse:
            double angle = gen_(glm::two_pi<double>());
            double normal_deviation = gen_();
            double normal_deviation_sq = std::sqrt(normal_deviation);
            glm::dvec3 u = glm::normalize(glm::cross(
                    std::abs(oriented_normal.x) < std::abs(oriented_normal.y) ? glm::dvec3{1, 0, 0}
                                                                              : glm::dvec3{0, 1, 0},
                    oriented_normal));
            glm::dvec3 v = glm::cross(oriented_normal, u);
            glm::dvec3 direction =
                    glm::normalize(u * std::cos(angle) * normal_deviation_sq
                                   + v * std::sin(angle) * normal_deviation_sq
                                   + oriented_normal * std::sqrt(1 - normal_deviation));

            return shape.material.emission
                   + shape.material.color * radiance({intersection, direction}, scene, depth + 1);
        }

        return {0, 1, 0};
    }
}  // namespace pathtracer
