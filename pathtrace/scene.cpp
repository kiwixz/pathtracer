#include "pathtrace/scene.h"
#include "pathtrace/serializers.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace pathtracer {
    void Scene::load_from_json(const std::string& source)
    {
        *this = nlohmann::json::parse(source);
    }

    void Scene::load_from_file(const std::string& path)
    {
        std::ifstream ifs{path};
        if (!ifs)
            throw std::runtime_error{"could not open scene file"};

        ifs.seekg(0, std::ios::end);
        size_t scene_size = ifs.tellg();
        ifs.seekg(0);

        std::string scene_json(scene_size, '\0');
        ifs.read(scene_json.data(), scene_size);

        return load_from_json(scene_json);
    }

    std::string Scene::save_to_json(int pretty_indent) const
    {
        nlohmann::json j = *this;
        return j.dump(pretty_indent);
    }

    void Scene::save_to_file(const std::string& path, int pretty_indent) const
    {
        std::ofstream ofs{path};
        ofs << save_to_json(pretty_indent);
    }
}  // namespace pathtracer
