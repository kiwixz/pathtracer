#include "pathtrace/scene.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace pathtracer {
    void Scene::load_from_file(const std::string& path)
    {
        std::ifstream ifs(path);
        if (!ifs)
            throw std::runtime_error{"could not open scene file"};
        nlohmann::json json = nlohmann::json::parse(ifs);
        nlohmann::json j;

        j = json["settings"];
        settings.width = j["width"];
        settings.height = j["height"];
    }

    void Scene::save_to_file(const std::string& path) const
    {
        // TODO
    }
}  // namespace pathtracer
