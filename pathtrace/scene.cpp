#include "scene.h"
#include <pathtrace/serializers.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>

namespace pathtrace {
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
        ifs.close();

        std::filesystem::path cd = std::filesystem::current_path();
        std::filesystem::path new_cd = std::filesystem::canonical(std::filesystem::path{path}).parent_path();
        std::filesystem::current_path(new_cd);  // change directory e.g. to load relative mesh files

        load_from_json(scene_json);

        std::filesystem::current_path(cd);
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
}  // namespace pathtrace
