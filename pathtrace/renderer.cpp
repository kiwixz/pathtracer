#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "pathtrace/fast_rand.h"
#include "pathtrace/renderer.h"
#include <glm/mat4x4.hpp>
#include <future>

namespace pathtracer {
    namespace {
        /// keeps references given in constructor
        class RendererWork {
        public:
            RendererWork(std::vector<Color>& pixels, const Scene& scene);

            void operator()(int x_min, int x_max, int y_min, int y_max);

        private:
            std::vector<Color>& pixels_;
            const Scene& scene_;

            FastRand gen_;

            Color radiance(const Ray& ray, int depth = 0);
        };


        constexpr Color background_color{0, 0, 0};


        RendererWork::RendererWork(std::vector<Color>& pixels, const Scene& scene) :
            pixels_{pixels}, scene_{scene}
        {}

        /// max are not inclusive
        void RendererWork::operator()(int x_min, int x_max, int y_min, int y_max)
        {
            glm::dmat4 projection = glm::yawPitchRoll(
                    scene_.camera.rotation.x, scene_.camera.rotation.y, scene_.camera.rotation.z);
            projection[0][3] = scene_.camera.position.x;
            projection[1][3] = scene_.camera.position.y;
            projection[2][3] = scene_.camera.position.z;

            double x_delta = 1.0 / scene_.settings.width;
            double y_delta = 1.0 / scene_.settings.height;

            for (int y = y_min; y < y_max; ++y) {
                for (int x = x_min; x < x_max; ++x) {
                    Color& color = pixels_[y * scene_.settings.width + x];
                    color = {0, 0, 0};
                    glm::dvec3 pixel_on_screen{
                            (x * x_delta - .5) * scene_.settings.width / scene_.settings.height,
                            .5 - y * y_delta, scene_.camera.focal};
                    for (int i = 0; i < scene_.settings.samples; ++i) {
                        glm::dvec4 point_on_screen{pixel_on_screen.x + gen_(x_delta),
                                pixel_on_screen.y + gen_(y_delta), pixel_on_screen.z, 1};
                        glm::dvec3 eye_to_screen = projection * point_on_screen;
                        Ray ray{scene_.camera.position, glm::normalize(eye_to_screen)};
                        color += radiance(ray) / static_cast<float>(scene_.settings.samples);
                    }
                }
            }
        }

        Color RendererWork::radiance(const Ray& ray, int depth)
        {
            const Shape* shape_ptr = nullptr;
            double distance = std::numeric_limits<double>::infinity();
            for (const std::unique_ptr<Shape>& s : scene_.shapes) {
                std::optional<double> new_distance = s->intersect(ray);

                if (new_distance < 1e-9)  // TODO justify epsilon
                    new_distance.reset();

                if (new_distance && new_distance < distance) {
                    distance = new_distance.value();
                    shape_ptr = s.get();
                }
            }

            if (!shape_ptr)
                return background_color;

            const Shape& shape = *shape_ptr;       // for convenience
            const Material& mat = shape.material;  //

            if (depth >= scene_.settings.max_bounces)  // TODO russian roulette with color
                return shape.material.emission;
            ++depth;

            glm::dvec3 intersection = ray.origin + ray.direction * distance;
            glm::dvec3 normal = shape.normal(intersection);
            glm::dvec3 oriented_normal = glm::dot(normal, ray.direction) > 0 ? -normal : normal;

            switch (shape.material.reflection) {
            case Material::Reflection::diffuse: {
                double angle = gen_(glm::two_pi<double>());
                double normal_deviation = gen_();
                double normal_deviation_sq = std::sqrt(normal_deviation);

                // build unit hemisphere frame: (u, v, oriented_normal)
                glm::dvec3 u = glm::normalize(
                        glm::cross(std::abs(oriented_normal.x) < std::abs(oriented_normal.y)
                                           ? glm::dvec3{1, 0, 0}
                                           : glm::dvec3{0, 1, 0},
                                oriented_normal));
                glm::dvec3 v = glm::cross(oriented_normal, u);

                glm::dvec3 direction =
                        glm::normalize(u * std::cos(angle) * normal_deviation_sq
                                       + v * std::sin(angle) * normal_deviation_sq
                                       + oriented_normal * std::sqrt(1 - normal_deviation));
                return mat.emission + mat.color * radiance({intersection, direction}, depth);
            }
            case Material::Reflection::specular:
                return mat.emission
                       + mat.color
                                 * radiance({intersection, glm::reflect(ray.direction, normal)},
                                           depth);
            case Material::Reflection::refractive: {
                Ray reflected = {intersection, glm::reflect(ray.direction, normal)};
                bool into = (normal == oriented_normal);
                double eta_out =
                        1;  // TODO should be eta of smallest volume containing intersection
                double eta_in = 1.5;  // TODO add to material
                double eta_ratio = into ? eta_out / eta_in : eta_in / eta_out;
                double cos_normal = glm::dot(ray.direction, oriented_normal);
                double cos_transmission_sq =
                        1 - eta_ratio * eta_ratio * (1 - cos_normal * cos_normal);
                if (cos_transmission_sq <= 0)  // total internal reflection
                    return mat.emission + mat.color * radiance(reflected, depth);

                double cos_transmission = std::sqrt(cos_transmission_sq);
                Ray refracted = {intersection,
                        glm::normalize(ray.direction * eta_ratio
                                       - normal
                                                 * ((cos_normal * eta_ratio + cos_transmission)
                                                           * (into ? 1 : -1)))};
                double eta_difference = eta_in - eta_out;
                double eta_sum = eta_in + eta_out;
                double reflectance_normal = (eta_difference * eta_difference) / (eta_sum * eta_sum);
                double inverted_cos_theta =
                        1 - (into ? -cos_normal : glm::dot(refracted.direction, normal));
                double reflectance = reflectance_normal
                                     + (1 - reflectance_normal)
                                               * std::pow(inverted_cos_theta,
                                                         5);  // fresnel schlick approximation
                return mat.emission
                       + mat.color
                                 * (gen_() < reflectance ? radiance(reflected, depth)
                                                         : radiance(refracted, depth));
            }
            }

            throw std::runtime_error{"unknown material reflection: "
                                     + std::to_string(static_cast<int>(mat.reflection))};
        }
    }  // namespace


    Image Renderer::render(const Scene& scene, unsigned nr_threads_)
    {
        int nr_threads = (nr_threads_ ? nr_threads_ : std::thread::hardware_concurrency());
        if (!nr_threads)
            throw std::runtime_error{"could not guess number of threads"};

        std::vector<Color> pixels(scene.settings.width * scene.settings.height);
        std::vector<std::future<void>> work;

        if (scene.settings.width >= scene.settings.height) {
            // distribute work by columns
            if (nr_threads > scene.settings.width)
                nr_threads = scene.settings.width;
            int x_min = 0;
            for (int rem_threads = nr_threads; rem_threads > 1; --rem_threads) {
                int x_max = x_min + (scene.settings.width - x_min) / rem_threads;
                work.emplace_back(std::async(std::launch::async, RendererWork{pixels, scene}, x_min,
                        x_max, 0, scene.settings.height));
                x_min = x_max;
            }
            work.emplace_back(std::async(std::launch::async, RendererWork{pixels, scene}, x_min,
                    scene.settings.width, 0, scene.settings.height));
        }
        else {
            // distribute work by rows
            if (nr_threads > scene.settings.height)
                nr_threads = scene.settings.height;
            int y_min = 0;
            for (int rem_threads = nr_threads; rem_threads > 1; --rem_threads) {
                int y_max = y_min + (scene.settings.height - y_min) / rem_threads;
                work.emplace_back(std::async(std::launch::async, RendererWork{pixels, scene}, 0,
                        scene.settings.width, y_min, y_max));
                y_min = y_max;
            }
            work.emplace_back(std::async(std::launch::async, RendererWork{pixels, scene}, 0,
                    scene.settings.width, y_min, scene.settings.height));
        }

        for (std::future<void>& future : work)
            future.get();
        return {std::move(pixels), scene.settings.width, scene.settings.height};
    }  // namespace pathtracer
}  // namespace pathtracer
