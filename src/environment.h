#pragma once

#include <nlohmann/json.hpp>
#include "util/util.hpp"

// program environment, should be a static instance somewhere

namespace Editor
{
    using EnvironmentPath = std::filesystem::path;
    using ApplicationJSON = std::pair<EnvironmentPath, nlohmann::json>;

    enum ApplicationJSON_
    {
        ApplicationJSON_Environment,
        ApplicationJSON_Names,
        ApplicationJSON_Overrides
    };

    enum EnvironmentPaths_
    {
        EnvironmentPaths_Dump,
        EnvironmentPaths_Root,
        EnvironmentPaths_Overlays,
        EnvironmentPaths_Stages,
        EnvironmentPaths_Characters,
        EnvironmentPaths_Palettes,
        EnvironmentPaths_Map16,
        EnvironmentPaths_Objects,
        EnvironmentPaths_Textures
    };

    struct Environment
    {
        std::vector<ApplicationJSON> jsons =
        {
            ApplicationJSON("settings/environment.json", nlohmann::json{}),
            ApplicationJSON("res/names.json", nlohmann::json{}),
            ApplicationJSON("res/overrides.json", nlohmann::json{}),
        };

        std::vector<EnvironmentPath> paths = std::vector<EnvironmentPath>(9);

        void load_jsons_from_disk(const EnvironmentPath& cwd);
        void save_jsons_to_disk(const EnvironmentPath& cwd);
        void load_paths_from_json();
        void save_paths_to_json();
        bool validate_paths();
    };
}