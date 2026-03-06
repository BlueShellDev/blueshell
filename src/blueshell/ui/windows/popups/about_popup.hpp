#pragma once
#include "imgui.h"
#include <string>

namespace BS::UI
{
   inline void ShowAboutPopup(const std::string &id, bool *is_open)
    {
        ImGui::OpenPopup(id.c_str());
        if (ImGui::BeginPopupModal(id.c_str(), is_open, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::BeginChild("##about_child", ImVec2(400, 300), true);
            ImGui::PushFont(ImGui::GetFont(), 26.0f);
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "BlueShell (early 2026)\n");
            ImGui::PopFont();
            ImGui::PushFont(ImGui::GetFont(), 18.0f);
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "A New! 'New! Super Mario Bros.' Editor\n");
            ImGui::PopFont();
            ImGui::SeparatorText("What is this?");
            ImGui::TextWrapped("BlueShell is a level editor for New Super Mario Bros. on the Nintendo DS. It is currently in early development and is not yet feature complete. \n\nThe goal of this project is to create a plugin-centric and powerful level editor that allows users to create and edit levels for NSMB with ease. \n\nIf you have any questions, suggestions, or want to contribute to the project, please visit the GitHub repository linked below.");
            ImGui::SeparatorText("Links");
            ImGui::BulletText("GitHub Repository: ");
            ImGui::SameLine();
            ImGui::TextLinkOpenURL("BlueShell", "https://github.com/BlueShellDev/blueshell");
            ImGui::EndChild();
            ImGui::EndPopup();
        };
    }
};