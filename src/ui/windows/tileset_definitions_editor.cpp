#include "tileset_definitions_editor.h"
#include "imgui.h"
#include "imfilebrowser.h"
#include "core/systems/io/binary_file.h"

void BS::UI::ShowTilesetDefinitionsEditor(bool &isOpen, BSContext &mainContext)
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(600, 325), ImVec2(800, 325));
    if (ImGui::Begin("Edit Tileset Definitions", &isOpen, ImGuiWindowFlags_MenuBar))
    {
        static enum class FileAction {
            Load,
            Save
        } action{FileAction::Load};
        static ImGui::FileBrowser fileDialog(ImGuiFileBrowserFlags_EnterNewFilename);
        fileDialog.SetTypeFilters({".bin"});

        static bool openTilesetTableDefinitionsPrompt = false;

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Load Definitions from Overlay..."))
                {
                    action = FileAction::Load;
                    fileDialog.SetTitle("Open Definitions Overlay...");
                    openTilesetTableDefinitionsPrompt = true;
                }
                if (ImGui::MenuItem("Save Definitions to Overlay..."))
                {
                    action = FileAction::Save;
                    fileDialog.SetTitle("Save Definitions Overlay...");
                    openTilesetTableDefinitionsPrompt = true;
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();

            if (openTilesetTableDefinitionsPrompt)
                ImGui::OpenPopup("Enter Table Definitions");
            if (ImGui::BeginPopupModal("Enter Table Definitions", &openTilesetTableDefinitionsPrompt, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::SetNextItemWidth(100.0f);
                ImGui::InputScalar("Entry Count", ImGuiDataType_U32, &mainContext.tilesetTableOffsets.entryCount);
                ImGui::SameLine();
                // function-ize the (?) tooltip it's used a lot in this popup and will be across the editor
                ImGui::Text("(?)");
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("The number of entries in the tileset index table. The default value is 76, which is the number of entries in the original NSMB. Do NOT change this value unless you know what you're doing!");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
                ImGui::SetNextItemWidth(100.0f);
                ImGui::InputScalar("NCG Base Offset", ImGuiDataType_U32, &mainContext.tilesetTableOffsets.NCGBaseOffset, nullptr, nullptr, "%08X");
                ImGui::SameLine();
                ImGui::Text("(?)");
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Base offset of the BG_ncg file IDs in the tileset index table. The default value is 0x00030EA4, which is the offset of the first NCG entry in the original NSMB. Do NOT change this value unless you know what you're doing!");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
                ImGui::SetNextItemWidth(100.0f);
                ImGui::InputScalar("NCL Base Offset", ImGuiDataType_U32, &mainContext.tilesetTableOffsets.NCLBaseOffset, nullptr, nullptr, "%08X");
                ImGui::SameLine();
                ImGui::Text("(?)");
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Base offset of the BG_ncl file IDs in the tileset index table. The default value is 0x000312A4, which is the offset of the first NCL entry in the original NSMB. Do NOT change this value unless you know what you're doing!");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
                ImGui::SetNextItemWidth(100.0f);
                ImGui::InputScalar("PNL Base Offset", ImGuiDataType_U32, &mainContext.tilesetTableOffsets.PNLBaseOffset, nullptr, nullptr, "%08X");
                ImGui::SameLine();
                ImGui::Text("(?)");
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Base offset of the BG_pnl file IDs in the tileset index table. The default value is 0x000312A4, which is the offset of the first PNL entry in the original NSMB. Do NOT change this value unless you know what you're doing!");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
                ImGui::SetNextItemWidth(100.0f);
                ImGui::InputScalar("UNT Base Offset", ImGuiDataType_U32, &mainContext.tilesetTableOffsets.UNTBaseOffset, nullptr, nullptr, "%08X");
                ImGui::SameLine();
                ImGui::Text("(?)");
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Base offset of the BG_unt file IDs in the tileset index table. The default value is 0x000316A4, which is the offset of the first UNT entry in the original NSMB. Do NOT change this value unless you know what you're doing!");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
                ImGui::SetNextItemWidth(100.0f);
                ImGui::InputScalar("CHK Base Offset", ImGuiDataType_U32, &mainContext.tilesetTableOffsets.CHKBaseOffset, nullptr, nullptr, "%08X");
                ImGui::SameLine();
                ImGui::Text("(?)");
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Base offset of the BG_chk file IDs in the tileset index table. The default value is 0x00031AA4, which is the offset of the first CHK entry in the original NSMB. Do NOT change this value unless you know what you're doing!");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }

                ImGui::Separator();
                if (ImGui::Button("Proceed", ImVec2(100, 0)))
                {
                    fileDialog.SetTitle("Save Definitions Overlay...");
                    fileDialog.Open();
                    ImGui::CloseCurrentPopup();
                    openTilesetTableDefinitionsPrompt = false;
                };
                ImGui::EndPopup();
            }

            fileDialog.Display();
            if (fileDialog.HasSelected())
            {
                std::filesystem::path selectedPath = fileDialog.GetSelected();
                fileDialog.ClearSelected();

                switch (action)
                {
                case FileAction::Load:
                    mainContext.tilesetDefinitions = BS::NSMB::ReadTilesetIndexFromOverlay0(BS::IO::ReadByteArrayFromFile(selectedPath), mainContext.tilesetTableOffsets);
                    BS::Console::Global().Log("Loaded tileset definitions from file: {}", selectedPath.string());
                    break;
                case FileAction::Save:
                    BS::NSMB::WriteTilesetIndexToOverlay0(mainContext.overlay0, mainContext.tilesetTableOffsets, mainContext.tilesetDefinitions);
                    BS::IO::WriteByteArrayToFile(selectedPath.string(), mainContext.overlay0);
                    BS::Console::Global().Log("Saved tileset definitions to file: {}", selectedPath.string());
                    break;
                }

                fileDialog.Close();
            }
        }

        static size_t selectedTilesetID = 0;
        if (ImGui::BeginChild("##tileset_list", ImVec2(200, 0), true))
        {
            for (const BS::NSMB::TilesetDefinition &tileset : mainContext.tilesetDefinitions)
            {
                bool selected = selectedTilesetID == tileset.id;
                ImGui::PushID(tileset.id);
                if (ImGui::Selectable(tileset.name.c_str(), &selected))
                {
                    selectedTilesetID = tileset.id;
                }

                ImGui::PopID();
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();
        if (ImGui::BeginChild("#tileset_details", ImVec2(0, 0), true))
        {
            if (mainContext.tilesetDefinitions.size() <= selectedTilesetID)
            {
                ImGui::TextWrapped("No tileset definitions found. Please load a valid tileset index from the File menu.");
                ImGui::EndChild();
                ImGui::End();
                return;
            }

            BS::NSMB::TilesetDefinition &selectedTileset = mainContext.tilesetDefinitions[selectedTilesetID];

            if (ImGui::BeginTable("##tileset_properties", 2,
                                  ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                const static float rowHeight = 0.0f;
                ImGui::TableNextRow(0, rowHeight);
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("ID");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%zu", selectedTileset.id);

                ImGui::TableNextRow(0, rowHeight);
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Name");
                ImGui::TableSetColumnIndex(1);
                char nameBuffer[64];
                std::strncpy(nameBuffer, selectedTileset.name.c_str(), sizeof(nameBuffer));
                ImGui::SetNextItemWidth(-1);
                if (ImGui::InputText("##name_input", nameBuffer, sizeof(nameBuffer)))
                    selectedTileset.name = nameBuffer;

                ImGui::TableNextRow(0, rowHeight);
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("NCG File ID");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-1);
                ImGui::InputScalar("##character_file_id_input", ImGuiDataType_U16, &selectedTileset.characterFileID);

                ImGui::TableNextRow(0, rowHeight);
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("NCL File ID");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-1);
                ImGui::InputScalar("##palette_file_id_input", ImGuiDataType_U16, &selectedTileset.paletteFileID);

                ImGui::TableNextRow(0, rowHeight);
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("PNL File ID");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-1);
                ImGui::InputScalar("##map16_file_id_input", ImGuiDataType_U16, &selectedTileset.map16FileID);

                ImGui::TableNextRow(0, rowHeight);
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("UNT File ID");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-1);
                ImGui::InputScalar("##object_file_id_input", ImGuiDataType_U16, &selectedTileset.objectFileID);

                ImGui::TableNextRow(0, rowHeight);
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("CHK File ID");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-1);
                ImGui::InputScalar("##tile_behavior_file_id_input", ImGuiDataType_U16, &selectedTileset.tileBehaviorFileID);

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}