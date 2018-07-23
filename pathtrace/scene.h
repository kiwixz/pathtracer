#pragma once

#include <string_view>

namespace pathtracer {
    struct Scene {
        struct Settings {
            int width = 0;
            int height = 0;
        };


        Settings settings;


        void load_from_file(const std::string& path);
        void save_to_file(const std::string& path) const;
    };
}  // namespace pathtracer
