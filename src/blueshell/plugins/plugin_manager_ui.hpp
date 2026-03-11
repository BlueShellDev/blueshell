#pragma once

#include <cstring>
#include <format>
#include <string>

#include "imgui.h"
#include "imgui-3rd-party/imfilebrowser.h"
#include "plugin_manager.h"

namespace BS::Plugins::UI
{
    inline void ShowPluginManagerWindow(const std::string &id, bool *is_open, PluginManager *manager)
    {
        if (manager == nullptr)
            return;

        static ImGui::FileBrowser import_dir_browser(ImGuiFileBrowserFlags_SelectDirectory);
        static std::string selected_plugin_id;
        static std::string selected_plugin_dir;
        static std::string last_action_message;

        ImGui::SetNextWindowSize(ImVec2(1000, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(id.c_str(), is_open, ImGuiWindowFlags_NoCollapse))
        {
            ImGui::End();
            return;
        }

        if (ImGui::Button("Rescan"))
        {
            std::string error;
            if (!manager->DiscoverPlugins(&error))
                last_action_message = std::format("Rescan failed: {}", error);
            else
                last_action_message = "Rescan complete";
        }

        ImGui::SameLine();
        if (ImGui::Button("Import Bundle"))
            import_dir_browser.Open();
        
        import_dir_browser.Display();
        if (import_dir_browser.HasSelected())
        {
            selected_plugin_dir = import_dir_browser.GetSelected();

            std::string error;
            if (!manager->ImportPluginBundle(selected_plugin_dir, &error))
                last_action_message = std::format("Import failed: {}", error);
            else
                last_action_message = "Import succeeded";

            import_dir_browser.ClearSelected();
            import_dir_browser.Close();
        }

        if (!selected_plugin_dir.empty())
        {
            ImGui::SameLine();
            ImGui::Text("Selected directory: '%s'", selected_plugin_dir.c_str());
        }

        if (!last_action_message.empty())
            ImGui::TextWrapped("%s", last_action_message.c_str());

        ImGui::Separator();

        ImGui::BeginChild("##plugin_list", ImVec2(800, 0), true);
        if (ImGui::BeginTable(
                "##plugins_table",
                5,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Enabled", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableHeadersRow();

            for (const std::string &plugin_id : manager->GetPluginIdsSorted())
            {
                const PluginRecord *record = manager->FindPlugin(plugin_id);
                if (record == nullptr)
                    continue;

                const bool is_selected = (selected_plugin_id == plugin_id);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                const char *display_name = record->manifest.has_value() ? record->manifest->name.c_str() : "<invalid manifest>";
                if (ImGui::Selectable(display_name, is_selected, ImGuiSelectableFlags_SpanAllColumns))
                    selected_plugin_id = plugin_id;

                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(plugin_id.c_str());

                ImGui::TableSetColumnIndex(2);
                if (record->manifest.has_value())
                    ImGui::TextUnformatted(record->manifest->version.c_str());
                else
                    ImGui::TextUnformatted("-");

                ImGui::TableSetColumnIndex(3);
                ImGui::TextUnformatted(ToString(record->status));

                ImGui::TableSetColumnIndex(4);
                ImGui::TextUnformatted(record->grants.enabled ? "Yes" : "No");
            }

            ImGui::EndTable();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("##plugin_details", ImVec2(0, 0), true);
        if (!selected_plugin_id.empty())
        {
            PluginRecord *record = manager->FindPluginMutable(selected_plugin_id);
            if (record != nullptr)
            {
                ImGui::Text("ID: %s", record->id.c_str());
                ImGui::Text("Status: %s", ToString(record->status));
                if (!record->statusMessage.empty())
                    ImGui::TextWrapped("Message: %s", record->statusMessage.c_str());

                if (record->manifest.has_value())
                {
                    ImGui::Text("Name: %s", record->manifest->name.c_str());
                    ImGui::Text("Version: %s", record->manifest->version.c_str());
                    ImGui::Text("Entry: %s", record->manifest->entryRelativePath.string().c_str());

                    bool enabled = record->grants.enabled;
                    if (ImGui::Checkbox("Enabled", &enabled))
                    {
                        std::string error;
                        if (!manager->SetPluginEnabled(record->id, enabled, &error))
                            last_action_message = std::format("Enable toggle failed: {}", error);
                        else
                            last_action_message = "Plugin enabled state updated";
                    }

                    if (record->status == PluginStatus::Loaded)
                    {
                        ImGui::SameLine();
                        if (ImGui::Button("Unload"))
                        {
                            std::string error;
                            if (!manager->UnloadPlugin(record->id, &error))
                                last_action_message = std::format("Unload failed: {}", error);
                            else
                                last_action_message = "Plugin unloaded";
                        }
                    }
                    else
                    {
                        ImGui::SameLine();
                        if (ImGui::Button("Load"))
                        {
                            std::string error;
                            if (!manager->LoadPlugin(record->id, &error))
                                last_action_message = std::format("Load failed: {}", error);
                            else
                                last_action_message = "Plugin loaded";
                        }
                    }

                    ImGui::SeparatorText("Declared APIs");
                    for (const PluginApiName api_name : record->manifest->declaredApis)
                    {
                        ImGui::BulletText("%s (requires %s)", ToString(api_name), ToString(RequiredPermissionForApi(api_name)));
                    }

                    ImGui::SeparatorText("Permissions");
                    for (const PluginPermission permission : record->manifest->declaredPermissions)
                    {
                        bool allow = IsPermissionGranted(record->grants, permission);
                        if (ImGui::Checkbox(ToString(permission), &allow))
                        {
                            std::string error;
                            if (!manager->SetPermission(record->id, permission, allow, &error))
                                last_action_message = std::format("Permission update failed: {}", error);
                            else
                                last_action_message = std::format("Permission '{}' updated", ToString(permission));
                        }
                    }

                    if (IsDebugUndeclaredApiOverrideSupported())
                    {
                        ImGui::SeparatorText("Debug Overrides");
                        bool allow_undeclared = record->grants.allowUndeclaredApisForDebug;
                        if (ImGui::Checkbox("Allow undeclared API usage (debug only)", &allow_undeclared))
                        {
                            std::string error;
                            if (!manager->SetDebugOverride(record->id, allow_undeclared, &error))
                                last_action_message = std::format("Debug override update failed: {}", error);
                            else
                                last_action_message = "Debug override updated";
                        }
                    }
                }

                ImGui::SeparatorText("Diagnostics");
                ImGui::Text("Denied API calls: %u", record->diagnostics.deniedCallsTotal);
                for (const auto &[api_name, count] : record->diagnostics.deniedCallsByApi)
                {
                    ImGui::BulletText("%s: %u", ToString(api_name), count);
                }
                if (!record->diagnostics.lastError.empty())
                    ImGui::TextWrapped("Last error: %s", record->diagnostics.lastError.c_str());
            }
            else
            {
                ImGui::TextUnformatted("Selected plugin no longer exists. Rescan to refresh.");
            }
        }
        else
        {
            ImGui::TextUnformatted("Select a plugin from the list to review details.");
        }

        ImGui::EndChild();
        ImGui::End();
    }
}
