#pragma once
#include "imgui.h"
#include "blueshell/blueshell.h"

namespace BS::UI
{
    inline void ShowDebugWindow(const std::string& id, bool *is_open, BS::MainContext* pgm_ctx)
    {
        ImGui::Begin(id.c_str(), is_open, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::BeginChild("##debug_mainchild", ImVec2(400, 200), true);
        ImGui::BulletText("Framerate [ImGUi::GetIO().Framerate]: %.0f fps", pgm_ctx->imgui_ctx->IO.Framerate);
        ImGui::EndChild();
        ImGui::End();
    };
}