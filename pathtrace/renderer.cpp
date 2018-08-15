#include "pathtrace/renderer.h"
#include "pathtrace/fast_rand.h"
#include "pathtrace/math.h"
#include <glm/gtc/constants.hpp>
#include <future>

namespace pathtrace {
    namespace {
        /// be careful: keeps references given in constructor
        class RendererWork {
        public:
            RendererWork(std::vector<Color>& pixels, const Scene& scene);

            void operator()(int x_min, int x_max, int y_min, int y_max);

        private:
            std::vector<Color>& pixels_;
            const Scene& scene_;

            FastRand gen_;

            Intersection intersect(const Ray& ray);
            Color radiance(const Ray& ray, int depth = 0);
        };

        RendererWork::RendererWork(std::vector<Color>& pixels, const Scene& scene) :
            pixels_{pixels}, scene_{scene}
        {}

        /// max are not inclusive
        void RendererWork::operator()(int x_min, int x_max, int y_min, int y_max)
        {
            glm::dvec3 fixed_rotation{scene_.camera.rotation.x,
                                      -scene_.camera.rotation.y,
                                      scene_.camera.rotation.z};
            glm::dmat4 projection = glm::transpose(math::transform(scene_.camera.position, fixed_rotation));
            double aspect_ratio = scene_.settings.width / static_cast<double>(scene_.settings.height);
            double fov_ratio = 2 * tan(scene_.camera.field_of_view / 2);

            double plane_x_delta = 1.0 / scene_.settings.width;
            double plane_y_delta = 1.0 / scene_.settings.height;
            double plane_x_ratio = aspect_ratio * fov_ratio;
            double plane_y_ratio = fov_ratio;

            for (int y = y_min; y < y_max; ++y) {
                for (int x = x_min; x < x_max; ++x) {
                    Color& color = pixels_[y * scene_.settings.width + x];
                    color = {0, 0, 0};

                    glm::dvec3 pixel_on_screen{(x * plane_x_delta - .5) * plane_x_ratio,
                                               (.5 - y * plane_y_delta) * plane_y_ratio,
                                               -1};
                    for (int i = 0; i < scene_.settings.samples; ++i) {
                        glm::dvec3 point_on_screen{pixel_on_screen.x + gen_(plane_x_delta) * plane_x_ratio,
                                                   pixel_on_screen.y + gen_(plane_y_delta) * plane_y_ratio,
                                                   pixel_on_screen.z};
                        glm::dvec3 eye_to_screen = projection * glm::dvec4{point_on_screen, 1};
                        Ray ray{scene_.camera.position, glm::normalize(eye_to_screen)};
                        color += radiance(ray) / static_cast<double>(scene_.settings.samples);
                    }
                }
            }
        }

        Intersection RendererWork::intersect(const Ray& ray)
        {
            Intersection intersection;

            for (const std::unique_ptr<Shape>& shape : scene_.shapes) {
                Intersection new_intersection = shape->intersect(ray);

                if (new_intersection.distance < 1e-9)  // TODO justify epsilon
                    new_intersection.shape = nullptr;

                if (new_intersection && new_intersection.distance < intersection.distance) {
                    intersection = std::move(new_intersection);
                }
            }

            return intersection;
        }

        Color RendererWork::radiance(const Ray& ray, int depth)
        {
            Intersection intersection = intersect(ray);
            if (!intersection)
                return scene_.settings.background_color;

            const Shape& shape = *intersection.shape;
            const glm::dvec3& point = intersection.point;
            const glm::dvec3& normal = intersection.normal;
            const Material& mat = shape.material;

            Color color = mat.color;

            if (depth >= scene_.settings.min_bounces) {
                double color_max = std::max(color.x, std::max(color.y, color.z));
                double survive_probability = color_max * (scene_.settings.max_bounces - depth) / scene_.settings.max_bounces;
                if (gen_() < survive_probability)
                    color /= color_max;
                else
                    return mat.emission;
            }
            ++depth;

            glm::dvec3 oriented_normal = glm::dot(normal, ray.direction) > 0 ? -normal : normal;

            switch (shape.material.reflection) {
            case Material::Reflection::diffuse: {
                double angle = gen_(glm::two_pi<double>());
                double normal_deviation = gen_();
                double normal_deviation_sq = std::sqrt(normal_deviation);

                // build unit hemisphere frame: (u, v, oriented_normal)
                glm::dvec3 u = glm::normalize(glm::cross(
                        std::abs(oriented_normal.x) < std::abs(oriented_normal.y) ? glm::dvec3{1, 0, 0}
                                                                                  : glm::dvec3{0, 1, 0},
                        oriented_normal));
                glm::dvec3 v = glm::cross(oriented_normal, u);

                glm::dvec3 direction = glm::normalize(
                        u * std::cos(angle) * normal_deviation_sq
                        + v * std::sin(angle) * normal_deviation_sq
                        + oriented_normal * std::sqrt(1 - normal_deviation));
                return mat.emission + color * radiance({point, direction}, depth);
            }
            case Material::Reflection::specular:
                return mat.emission + color * radiance({point, glm::reflect(ray.direction, normal)}, depth);
            case Material::Reflection::refractive: {
                Ray reflected = {point, glm::reflect(ray.direction, normal)};
                bool into = (normal == oriented_normal);
                constexpr double eta_out = 1;  // indice of refraction of air
                double eta_in = 1.5;           // TODO add to material
                double eta_ratio = into ? eta_out / eta_in : eta_in / eta_out;
                double cos_normal = glm::dot(ray.direction, oriented_normal);
                double cos_transmission_sq = 1 - eta_ratio * eta_ratio * (1 - cos_normal * cos_normal);
                if (cos_transmission_sq <= 0)  // total internal reflection
                    return mat.emission + color * radiance(reflected, depth);

                double cos_transmission = std::sqrt(cos_transmission_sq);
                glm::dvec3 refraction_vec = ray.direction * eta_ratio - normal * ((cos_normal * eta_ratio + cos_transmission) * (into ? 1 : -1));
                Ray refracted = {point, glm::normalize(refraction_vec)};
                double eta_difference = eta_out - eta_in;
                double eta_sum = eta_out + eta_in;
                double reflectance_normal = (eta_difference * eta_difference) / (eta_sum * eta_sum);
                double inverted_cos_theta = 1 - (into ? -cos_normal : glm::dot(refracted.direction, normal));
                double reflectance = reflectance_normal + (1 - reflectance_normal) * std::pow(inverted_cos_theta, 5);  // fresnel schlick approximation
                return mat.emission + color * (gen_() < reflectance ? radiance(reflected, depth) : radiance(refracted, depth));
            }
            }

            throw std::runtime_error{"unknown material reflection: " + std::to_string(static_cast<int>(mat.reflection))};
        }
    }  // namespace


    Image Renderer::render(const Scene& scene, unsigned nr_threads_)
    {
        int nr_threads = (nr_threads_ ? nr_threads_ : std::thread::hardware_concurrency());
        if (!nr_threads)
            throw std::runtime_error{"could not guess number of threads"};

        for (const std::unique_ptr<Shape>& shape : scene.shapes)
            shape->bake();

        std::vector<Color> pixels(scene.settings.width * scene.settings.height);
        std::vector<std::future<void>> work;

        if (scene.settings.width >= scene.settings.height) {
            // distribute work by columns
            if (nr_threads > scene.settings.width)
                nr_threads = scene.settings.width;
            int x_min = 0;
            for (int rem_threads = nr_threads; rem_threads > 1; --rem_threads) {
                int x_max = x_min + (scene.settings.width - x_min) / rem_threads;
                work.emplace_back(std::async(std::launch::async,
                                             RendererWork{pixels, scene},
                                             x_min, x_max, 0, scene.settings.height));
                x_min = x_max;
            }
            work.emplace_back(std::async(std::launch::async,
                                         RendererWork{pixels, scene},
                                         x_min, scene.settings.width, 0, scene.settings.height));
        }
        else {
            // distribute work by rows
            if (nr_threads > scene.settings.height)
                nr_threads = scene.settings.height;
            int y_min = 0;
            for (int rem_threads = nr_threads; rem_threads > 1; --rem_threads) {
                int y_max = y_min + (scene.settings.height - y_min) / rem_threads;
                work.emplace_back(std::async(std::launch::async,
                                             RendererWork{pixels, scene},
                                             0, scene.settings.width, y_min, y_max));
                y_min = y_max;
            }
            work.emplace_back(std::async(std::launch::async,
                                         RendererWork{pixels, scene},
                                         0, scene.settings.width, y_min, scene.settings.height));
        }

        for (std::future<void>& future : work)
            future.get();
        return {std::move(pixels), scene.settings.width, scene.settings.height};
    }
}  // namespace pathtrace
