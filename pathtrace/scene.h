#pragma once

#include "pathtrace/shape.h"
#include <memory>
#include <string>
#include <vector>

namespace pathtracer {
    struct Scene {
        struct Settings {
            int width = 1;
            int height = 1;
            int samples = 1;
            int max_bounces = 1;
        };

        struct Camera {
            glm::dvec3 position;
            glm::dvec3 rotation;
            double focal;
        };


        Settings settings;
        Camera camera;
        std::vector<std::unique_ptr<Shape>> shapes;


        void load_from_file(const std::string& path);
        void save_to_file(const std::string& path) const;
    };
}  // namespace pathtracer
