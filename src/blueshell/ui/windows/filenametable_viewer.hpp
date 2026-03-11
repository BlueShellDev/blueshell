#pragma once
#include "imgui.h"
#include "nitro/simple_nitro.h"

namespace BS::UI
{
    inline void ShowFileNameTableViewer(const std::string& id, bool* is_open, 
        const Nitro::FS::FileNameTable& fn_table)
    {
        ImGui::Begin(id.c_str(), is_open, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::BulletText("Total files: %zu", fn_table.size());
        ImGui::SeparatorText("Nametable Files");
        ImGui::BeginChild("#filetable_child");
        if(ImGui::BeginTable("#filetable_table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("File ID (u16)", ImGuiTableColumnFlags_WidthFixed, 50.f);
            ImGui::TableSetupColumn("Filename");
            ImGui::TableHeadersRow();
            for (auto [id, fn] : fn_table)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", id);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", fn.c_str());
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
        ImGui::End();
    }
}