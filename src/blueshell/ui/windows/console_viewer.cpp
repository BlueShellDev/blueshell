#include "console_viewer.h"
#include "imgui.h"
#include "core/console.h"

void BS::UI::ShowConsoleWindow(bool &isOpen)
{
    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
    static bool autoScroll = true;
    ImGui::Begin("Console", &isOpen, flags);
    ImGui::Checkbox("Auto-Scroll", &autoScroll);

    ImGui::SameLine();
    if(ImGui::Button("Send [LOG] test..."))
        Core::Console::Global().Log("Everything seems fine!");
    ImGui::SameLine();
    if(ImGui::Button("Send [WAR] test..."))
        Core::Console::Global().Warn("Issues have arised...");
    ImGui::SameLine();
    if(ImGui::Button("Send [ERR] test..."))
        Core::Console::Global().Err("Critical error! (not really, this is just a drill)");

    ImGui::Separator();
    if (ImGui::BeginChild("##display", ImVec2(0, 0), true))
    {
        for (auto& msg : Core::Console::Global().buffer)
        {
            switch (msg.type)
            {
                case Core::ConsoleMsg::Type::Warning:
                    ImGui::TextColored(ImVec4(1.0, 1.0, 0.5, 1.0), "%s", msg.text.c_str());
                    break;
                case Core::ConsoleMsg::Type::Error:
                    ImGui::TextColored(ImVec4(1.0, 0.5, 0.5, 1.0), "%s", msg.text.c_str());
                    break;
                default:             
                    ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "%s", msg.text.c_str());
                    break;
            }
        }
        
        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
    ImGui::End();
}