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
            double field_of_view = glm::radians(90.0);
        };


        Settings settings;
        Camera camera;
        std::vector<std::unique_ptr<Shape>> shapes;


        Scene() = default;

        void load_from_json(const std::string& source);
        void load_from_file(const std::string& path);
        std::string save_to_json(int pretty_indent = -1) const;
        void save_to_file(const std::string& path, int pretty_indent = -1) const;
    };
}  // namespace pathtracer
