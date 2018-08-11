#pragma once

#include "pathtrace/image.h"
#include "pathtrace/shape.h"
#include <glm/trigonometric.hpp>
#include <memory>
#include <string>
#include <vector>

namespace pathtracer {
    struct Scene {
        struct Settings {
            int width = 1920;
            int height = 1080;
            int samples = 1;
            int max_bounces = 1;
            Color background_color;
        };

        struct Camera {
            glm::dvec3 position;
            glm::dvec3 rotation;
            double fov = glm::radians(90.0);
        };


        Settings settings;
        Camera camera;
        std::vector<std::unique_ptr<Shape>> shapes;


        void load_from_file(const std::string& path);
        void save_to_file(const std::string& path) const;
    };
}  // namespace pathtracer
