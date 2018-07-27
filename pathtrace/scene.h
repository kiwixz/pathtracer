#pragma once

#include "pathtrace/shape.h"
#include <memory>
#include <string>
#include <vector>

namespace pathtracer {
    struct Scene {
        struct Settings {
            int width = 0;
            int height = 0;
            int samples = 0;
        };

        struct Camera {
            glm::vec3 position;
            glm::vec3 rotation;
            float focal;
        };


        Settings settings;
        Camera camera;
        std::vector<std::unique_ptr<Shape>> shapes;


        void load_from_file(const std::string& path);
        void save_to_file(const std::string& path) const;
    };
}  // namespace pathtracer
