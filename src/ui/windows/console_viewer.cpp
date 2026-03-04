#include "console_viewer.h"
#include "imgui.h"
#include "core/systems/console/console.h"

void BS::UI::ShowConsoleWindow(bool &isOpen)
{
    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
    static bool autoScroll = true;
    ImGui::Begin("Console", &isOpen, flags);
    ImGui::Checkbox("Auto-Scroll", &autoScroll);
    ImGui::Separator();
    if (ImGui::BeginChild("##display", ImVec2(0, 0), true))
    {
        ImGui::TextUnformatted(BS::Console::Global().buffer.c_str());
        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
    ImGui::End();
}