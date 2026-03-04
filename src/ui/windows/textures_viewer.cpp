#include "textures_viewer.h"
#include "imgui.h"
#include "core/systems/render/render.h"

void BS::UI::ShowTexturesWindow(bool &isOpen)
{
    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
    ImGui::SetNextWindowSizeConstraints(ImVec2(600, 600), ImVec2(FLT_MAX, FLT_MAX));

    ImGui::Begin("Texture Viewer", &isOpen, flags);
    static const BS::Render::TextureMap &textures = BS::Render::GetTextures();
    static std::string selectedTextureName;
    ImGui::SetNextItemWidth(200);
    if (ImGui::BeginCombo("##texture_combo", selectedTextureName.empty() ? "Select a texture" : selectedTextureName.c_str(), ImGuiComboFlags_HeightSmall))
    {
        if (ImGui::Selectable("[None]", selectedTextureName.empty()))
            selectedTextureName.clear();
        for (const auto &[name, texture] : textures)
        {
            ImGui::PushID(name.c_str());
            if (ImGui::Selectable(name.c_str(), selectedTextureName == name))
                selectedTextureName = name;
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::Text("(%zu textures loaded)", textures.size());
    static size_t zoomLevel = 1;
    if (ImGui::Button("Zoom In"))
        zoomLevel *= 2;
    ImGui::SameLine();
    if (ImGui::Button("Zoom Out") && zoomLevel > 1)
        zoomLevel *= 0.5;
    ImGui::SameLine();
    ImGui::Separator();

    static ImVec2 viewerSize(0, 0);
    if (ImGui::BeginChild("##texture_frame", viewerSize, true))
    {
        ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImGui::GetCursorScreenPos(),
                                                            ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetWindowSize().x, ImGui::GetCursorScreenPos().y + ImGui::GetWindowSize().y),
                                                            IM_COL32(0, 0, 100, 255),
                                                            IM_COL32(0, 0, 100, 255),
                                                            IM_COL32(0, 0, 0, 255),
                                                            IM_COL32(0, 0, 0, 255));
        if (textures.find(selectedTextureName) != textures.end() && BS::Render::IsTextureValid(textures.at(selectedTextureName)))
        {
            const auto &texture = textures.at(selectedTextureName);
            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - texture.width * zoomLevel) / 2.0f, (ImGui::GetWindowSize().y - texture.height * zoomLevel) / 2.0f));
            ImGui::Image(ImTextureID((uintptr_t)texture.id), ImVec2(texture.width * zoomLevel, texture.height * zoomLevel));
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Dimensions: %d x %d", texture.width, texture.height);
                ImGui::Text("Format: %s", texture.format == BS::Render::Texture::Format::RGBA8888 ? "RGBA8888" : "RGB555");
                ImGui::EndTooltip();
            }
        }
        else
        {
            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - 100) / 2.0f, (ImGui::GetWindowSize().y - 20) / 2.0f));
            ImGui::Text("No signal.");
        }
    }
    ImGui::EndChild();
    ImGui::End();
}