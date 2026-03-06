#include "plugin_manifest.h"

#include <exception>
#include <format>
#include <regex>
#include <sstream>

#include "yaml-cpp/yaml.h"

namespace
{
    constexpr const char *kPlatformKey =
#if defined(_WIN32)
        "windows";
#elif defined(__APPLE__)
        "macos";
#else
        "linux";
#endif

    bool PathIsWithin(const std::filesystem::path &base, const std::filesystem::path &candidate)
    {
        const std::filesystem::path abs_base = std::filesystem::weakly_canonical(base);
        const std::filesystem::path abs_candidate = std::filesystem::weakly_canonical(candidate);

        auto it_base = abs_base.begin();
        auto it_candidate = abs_candidate.begin();
        for (; it_base != abs_base.end(); ++it_base, ++it_candidate)
        {
            if (it_candidate == abs_candidate.end() || *it_base != *it_candidate)
                return false;
        }

        return true;
    }

    bool IsValidPluginId(const std::string &value)
    {
        static const std::regex id_regex("^[A-Za-z0-9][A-Za-z0-9._-]*$");
        return std::regex_match(value, id_regex);
    }

    bool IsValidSemver(const std::string &value)
    {
        static const std::regex semver_regex(
            "^[0-9]+\\.[0-9]+\\.[0-9]+(?:-[0-9A-Za-z.-]+)?(?:\\+[0-9A-Za-z.-]+)?$");
        return std::regex_match(value, semver_regex);
    }

    template <typename T>
    bool ReadRequiredScalar(const YAML::Node &root, const char *key, T *out_value, std::string *error)
    {
        if (!root[key])
        {
            *error = std::format("Missing required key '{}'", key);
            return false;
        }

        try
        {
            *out_value = root[key].as<T>();
            return true;
        }
        catch (const YAML::Exception &ex)
        {
            *error = std::format("Failed to parse key '{}': {}", key, ex.what());
            return false;
        }
    }
}

BS::Plugins::PluginManifestLoadResult BS::Plugins::LoadPluginManifestFromPath(const std::filesystem::path &plugin_yaml_path)
{
    PluginManifestLoadResult result;

    try
    {
        if (!std::filesystem::exists(plugin_yaml_path))
        {
            result.error = std::format("Manifest does not exist: {}", plugin_yaml_path.string());
            return result;
        }

        const YAML::Node root = YAML::LoadFile(plugin_yaml_path.string());

        PluginManifest manifest;
        manifest.pluginDirectory = std::filesystem::weakly_canonical(plugin_yaml_path.parent_path());

        if (!ReadRequiredScalar(root, "id", &manifest.id, &result.error))
            return result;
        if (!IsValidPluginId(manifest.id))
        {
            result.error = std::format("Invalid plugin id '{}'", manifest.id);
            return result;
        }

        if (!ReadRequiredScalar(root, "name", &manifest.name, &result.error))
            return result;

        if (!ReadRequiredScalar(root, "version", &manifest.version, &result.error))
            return result;
        if (!IsValidSemver(manifest.version))
        {
            result.error = std::format("Version '{}' is not valid semver", manifest.version);
            return result;
        }

        if (!ReadRequiredScalar(root, "apiVersion", &manifest.apiVersion, &result.error))
            return result;
        if (manifest.apiVersion != BS_PLUGIN_ABI_VERSION)
        {
            result.error = std::format(
                "apiVersion {} is unsupported (expected {})",
                manifest.apiVersion,
                BS_PLUGIN_ABI_VERSION);
            return result;
        }

        if (!root["entry"] || !root["entry"].IsMap())
        {
            result.error = "Missing or invalid 'entry' mapping";
            return result;
        }

        if (!root["entry"][kPlatformKey])
        {
            result.error = std::format("Manifest entry map does not define '{}'", kPlatformKey);
            return result;
        }

        manifest.entryRelativePath = root["entry"][kPlatformKey].as<std::string>();
        if (manifest.entryRelativePath.empty())
        {
            result.error = "entry path is empty";
            return result;
        }

        manifest.entryAbsolutePath = std::filesystem::weakly_canonical(manifest.pluginDirectory / manifest.entryRelativePath);
        if (!PathIsWithin(manifest.pluginDirectory, manifest.entryAbsolutePath))
        {
            result.error = std::format("entry path escapes plugin root: {}", manifest.entryRelativePath.string());
            return result;
        }

        if (!root["apis"] || !root["apis"].IsSequence())
        {
            result.error = "Missing or invalid 'apis' sequence";
            return result;
        }
        for (const YAML::Node &node : root["apis"])
        {
            const std::string api_name = node.as<std::string>();
            const auto parsed = ParseApiName(api_name);
            if (!parsed.has_value())
            {
                result.error = std::format("Unknown API in manifest: {}", api_name);
                return result;
            }

            if (!manifest.declaredApiSet.contains(parsed.value()))
            {
                manifest.declaredApis.push_back(parsed.value());
                manifest.declaredApiSet.insert(parsed.value());
            }
        }

        if (!root["permissions"] || !root["permissions"].IsSequence())
        {
            result.error = "Missing or invalid 'permissions' sequence";
            return result;
        }

        for (const YAML::Node &node : root["permissions"])
        {
            const std::string permission_name = node.as<std::string>();
            const auto parsed = ParsePermission(permission_name);
            if (!parsed.has_value())
            {
                result.error = std::format("Unknown permission in manifest: {}", permission_name);
                return result;
            }

            if (!manifest.declaredPermissionSet.contains(parsed.value()))
            {
                manifest.declaredPermissions.push_back(parsed.value());
                manifest.declaredPermissionSet.insert(parsed.value());
            }
        }

        for (const PluginApiName api_name : manifest.declaredApis)
        {
            const PluginPermission required = RequiredPermissionForApi(api_name);
            if (!manifest.declaredPermissionSet.contains(required))
            {
                result.error = std::format(
                    "API '{}' requires missing permission '{}'",
                    ToString(api_name),
                    ToString(required));
                return result;
            }
        }

        result.manifest = std::move(manifest);
        result.ok = true;
    }
    catch (const YAML::Exception &ex)
    {
        result.error = std::format("YAML parse error: {}", ex.what());
    }
    catch (const std::exception &ex)
    {
        result.error = std::format("Manifest load error: {}", ex.what());
    }

    return result;
}
