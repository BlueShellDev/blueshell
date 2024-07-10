#include "environment.h"

namespace Editor
{
    void Environment::load_jsons_from_disk(const EnvironmentPath& cwd)
    {
        for (auto& json : jsons)
            json.second = Util::load_json_from_disk(cwd / json.first);
    }

    void Environment::save_jsons_to_disk(const EnvironmentPath& cwd)
    {
        for (auto& json : jsons)
            Util::save_json_to_disk(cwd / json.first, json.second);
    }

    void Environment::load_paths_from_json()
    {
        paths[EnvironmentPaths_Dump]        = jsons[ApplicationJSON_Environment].second["sys-paths"]["dump"].get<std::string>();
        paths[EnvironmentPaths_Root]        = jsons[ApplicationJSON_Environment].second["rom-fs"]["root"].get<std::string>();
        paths[EnvironmentPaths_Overlays]    = jsons[ApplicationJSON_Environment].second["rom-fs"]["overlays"].get<std::string>();
        paths[EnvironmentPaths_Stages]      = jsons[ApplicationJSON_Environment].second["rom-fs"]["stages"].get<std::string>();
        paths[EnvironmentPaths_Characters]  = jsons[ApplicationJSON_Environment].second["rom-fs"]["characters"].get<std::string>();
        paths[EnvironmentPaths_Palettes]    = jsons[ApplicationJSON_Environment].second["rom-fs"]["palettes"].get<std::string>();
        paths[EnvironmentPaths_Map16]       = jsons[ApplicationJSON_Environment].second["rom-fs"]["metatiles"].get<std::string>();
        paths[EnvironmentPaths_Objects]     = jsons[ApplicationJSON_Environment].second["rom-fs"]["objects"].get<std::string>();
        paths[EnvironmentPaths_Textures]    = jsons[ApplicationJSON_Environment].second["rom-fs"]["textures"].get<std::string>();
    }

    void Environment::save_paths_to_json()
    {
        jsons[ApplicationJSON_Environment].second["sys-paths"]["dump"]      = paths[EnvironmentPaths_Dump];
        jsons[ApplicationJSON_Environment].second["rom-fs"]["root"]         = paths[EnvironmentPaths_Root];
        jsons[ApplicationJSON_Environment].second["rom-fs"]["overlays"]     = paths[EnvironmentPaths_Overlays];
        jsons[ApplicationJSON_Environment].second["rom-fs"]["stages"]       = paths[EnvironmentPaths_Stages];
        jsons[ApplicationJSON_Environment].second["rom-fs"]["characters"]   = paths[EnvironmentPaths_Characters];
        jsons[ApplicationJSON_Environment].second["rom-fs"]["palettes"]     = paths[EnvironmentPaths_Palettes];
        jsons[ApplicationJSON_Environment].second["rom-fs"]["metatiles"]    = paths[EnvironmentPaths_Map16];
        jsons[ApplicationJSON_Environment].second["rom-fs"]["objects"]      = paths[EnvironmentPaths_Objects];
        jsons[ApplicationJSON_Environment].second["rom-fs"]["textures"]     = paths[EnvironmentPaths_Textures];
    }
}