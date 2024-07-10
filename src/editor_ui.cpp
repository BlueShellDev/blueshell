#include "editor.h"

namespace Editor
{
    void ApplicationUI::init_main_window
    (
        const std::string& title,
        const int& width,
        const int& height,
        const bool& maximize
    )
    {
        // basic boilerplate GLFW shit, just create a window and make sure it's there
        if (!glfwInit())
            throw std::runtime_error("failed to initialize GLFW!");

        // oh yeah, we need to check if we're running on mac and configure shaders accordingly for imgui
        // (copied right from the imgui demo thing)
#if defined(__APPLE__)
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

        // get window ptr and make sure it points to something
        main_window = glfwCreateWindow
        (
            width,
            height,
            title.c_str(),
            nullptr,
            nullptr
        );
        if (!main_window)
        {
            glfwTerminate();
            throw std::runtime_error("failed to create the main window!");
        }

        // maximize it if set
        if (maximize)
            glfwMaximizeWindow(main_window);

        // set window icon
        GLFWimage image;
        image.pixels = stbi_load("res/images/icon16.png", &image.width, &image.height, 0, 4);
        glfwSetWindowIcon(main_window, 1, &image);
        stbi_image_free(image.pixels);

        // create opengl and imgui contexts on window
        glfwMakeContextCurrent(main_window);
        ImGui::CreateContext();

        // init imgui opengl things
        ImGui_ImplGlfw_InitForOpenGL(main_window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void ApplicationUI::new_frame()
    {
        // gl refreshers
        glClearColor(0.4f, 0.5f, 0.6f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);

        // imgui refreshers
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ApplicationUI::init_dock_space()
    {
        // configure dock space
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, { 0, 0 });

        static bool startup = true;
        if (startup)
        {
            startup = false;
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            auto dock_id_down           = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.1f, nullptr, &dockspace_id);
            auto dock_id_left           = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.15f, nullptr, &dockspace_id);
            auto dock_id_left_down      = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.5f, nullptr, &dock_id_left);

            ImGui::DockBuilderDockWindow("AreaEditorWindow", dockspace_id);
            ImGui::DockBuilderDockWindow("ToolboxWindow", dock_id_left);
            ImGui::DockBuilderDockWindow("SelectionWindow", dock_id_left_down);
            ImGui::DockBuilderDockWindow("StatusWindow", dock_id_down);
            ImGui::DockBuilderFinish(dockspace_id);
        }
        ImGui::End();
    }

    void ApplicationUI::draw_main_menu_bar()
    {
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File"))
        {
#ifdef __APPLE__
            const char* open_shortcut = "CMD+O";
            const char* save_shortcut = "CMD+S";
#else
            const char* open_shortcut = "Ctrl+O";
            const char* save_shortcut = "Ctrl+S";
#endif
            if (ImGui::MenuItem("Open Level by Name...", open_shortcut))
                open_level_select_popup = true;
            ImGui::Separator();
            if (ImGui::MenuItem("Save Level", save_shortcut)) 
            {
                // TODO: why isn't this implemented yet??
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
#ifdef __APPLE__
            const char* copy_shortcut = "CMD+C";
            const char* paste_shortcut = "CMD+V";
            const char* delete_shortcut = "Delete";
            const char* cut_shortcut = "CMD+X";
#else
            const char* copy_shortcut = "Ctrl+C";
            const char* paste_shortcut = "Ctrl+V";
            const char* delete_shortcut = "Del";
            const char* cut_shortcut = "Ctrl+X";
#endif
            if (ImGui::MenuItem("Copy", copy_shortcut))
                area_editor.copy_selection();
            if (ImGui::MenuItem("Paste", paste_shortcut))
                area_editor.paste_selection();
            if (ImGui::MenuItem("Delete", delete_shortcut))
                area_editor.delete_selection();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
#ifdef __APPLE__
            const char* object_shortcut = "CMD+Option+O";
            const char* actor_shortcut = "CMD+Option+A";
            const char* entrance_shortcut = "CMD+Option+E";
            const char* view_shortcut = "CMD+Option+V";
            const char* path_shortcut = "CMD+Option+P";
            const char* zoom_in_shortcut = "CMD+=";
            const char* zoom_reset_shortcut = "CMD+0";
            const char* zoom_out_shortcut = "CMD+-";
#else
            const char* object_shortcut = "Ctrl+Alt+O";
            const char* actor_shortcut = "Ctrl+Alt+A";
            const char* entrance_shortcut = "Ctrl+Alt+E";
            const char* view_shortcut = "Ctrl+Alt+V";
            const char* path_shortcut = "Ctrl+Alt+P";
            const char* zoom_in_shortcut = "Ctrl+=";
            const char* zoom_reset_shortcut = "Ctrl+0";
            const char* zoom_out_shortcut = "Ctrl+-";
#endif
            ImGui::MenuItem("Show Objects", object_shortcut, &area_editor.show_objects);
            ImGui::MenuItem("Show Sprites", actor_shortcut, &area_editor.show_actors);
            ImGui::MenuItem("Show Entrances", entrance_shortcut, &area_editor.show_entrances);
            ImGui::MenuItem("Show Views/Zones", view_shortcut, &area_editor.show_views);
            ImGui::MenuItem("Show Paths", path_shortcut, &area_editor.show_paths);
            ImGui::Separator();
            if (ImGui::MenuItem("Scale +25%", zoom_in_shortcut))
                area_editor.scale(area_editor.draw_env.scale += 0.25);
            if (ImGui::MenuItem("Reset Scale", zoom_reset_shortcut))
                area_editor.scale(1.0f);
            if (ImGui::MenuItem("Scale -25%", zoom_out_shortcut))
                area_editor.scale(area_editor.draw_env.scale -= 0.25);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings"))
        {
            ImGui::ColorEdit4("Background color: ", &area_editor.background_color.x);
            ImGui::PushItemWidth(300.f);
            ImGui::ShowFontSelector("Font");
            ImGui::PopItemWidth();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    void ApplicationUI::draw_area_editor_window(const ImVec2& content_size_units)
    {
        ImGui::Begin("AreaEditorWindow");
        if (area_editor.area)
            area_editor.draw(&app->render_manager, content_size_units);
        ImGui::End();
    }

    void ApplicationUI::draw_toolbox_window()
    {
        ImGui::Begin("ToolboxWindow");
        ImGui::BeginTabBar("###toolbox_tabs");
        if (ImGui::BeginTabItem("Objects"))
        {
            if (area_editor.area)
            {
                ImGui::BeginTabBar("###tileset_tabs");
                for (auto& picker_widget : object_picker_widgets)
                    if (ImGui::BeginTabItem(picker_widget.display_name.c_str()))
                    {
                        ImGui::SliderFloat("Scale", &picker_widget.draw_env.scale, 1, 3, "%.0fx");

                        area_editor.paint_item = picker_widget.selection;
                        picker_widget.draw(&app->render_manager, { 50, 100 });
                        ImGui::EndTabItem();
                    }

                ImGui::EndTabBar();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
        ImGui::End();
    }

    void ApplicationUI::draw_status_window()
    {
        ImGui::Begin("StatusWindow", 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
        ImGui::Text(std::format("Zoom: {}x", area_editor.draw_env.scale).c_str());
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
        if (area_editor.area)
            ImGui::Text(std::format
                (
                    "Area {} of {} (PgUp/PgDown)",
                    area_editor.area->area_index + 1,
                    app->curr_level_source.areas.size()
                ).c_str());
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
        ImGui::Text(std::format("{} FPS", (int)ImGui::GetIO().Framerate).c_str());
        ImGui::End();
    }

    void ApplicationUI::draw_selection_window()
    {
        if (area_editor.selection.empty())
            return;

        const u16 step = 1;
        // only display properties for the first item in a multi-selection
        UI::AreaRenderItem* selection = area_editor.selection.at(0);

        ImGui::Begin("SelectionWindow");
        ImGui::BeginChild("##properties_child", { 0, 0 }, ImGuiChildFlags_Border);
        if (ImGui::CollapsingHeader("Item", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::BeginTable("##item_table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders))
            {
                ImGui::TableSetupColumn("Parameter");
                ImGui::TableSetupColumn("Value");
                ImGui::TableHeadersRow();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Item ID");
                ImGui::TableNextColumn();
                ImGui::PushItemWidth((ImGui::GetColumnWidth() - 8) / 2);
                ImGui::Text(std::format("{}", selection->item_id).c_str());
                ImGui::PopItemWidth();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Item Type");
                ImGui::TableNextColumn();
                ImGui::PushItemWidth((ImGui::GetColumnWidth() - 8) / 2);
                switch (selection->item_type)
                {
                case UI::AreaItemType_BgDat:
                    ImGui::Text("Object");
                    break;
                case UI::AreaItemType_StageObject:
                    ImGui::Text("Sprite");
                    break;
                case UI::AreaItemType_Entrance:
                    ImGui::Text("Entrance");
                    break;
                case UI::AreaItemType_View:
                    ImGui::Text("View");
                    break;
                case UI::AreaItemType_Zone:
                    ImGui::Text("Zone");
                    break;
                }
                ImGui::PopItemWidth();
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            if (ImGui::BeginTable("##transform_table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders))
            {
                ImGui::TableSetupColumn("Parameter");
                ImGui::TableSetupColumn("Value");
                ImGui::TableHeadersRow();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Position");
                ImGui::TableNextColumn();
                ImGui::PushItemWidth((ImGui::GetColumnWidth() - 8) / 2);
                ImGui::DragScalar("##x", ImGuiDataType_U16, &selection->transform.x, 0.25f);
                ImGui::SameLine();
                ImGui::DragScalar("##y", ImGuiDataType_U16, &selection->transform.y, 0.25f);
                ImGui::PopItemWidth();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Size");
                ImGui::TableNextColumn();
                ImGui::PushItemWidth((ImGui::GetColumnWidth() - 8) / 2);
                ImGui::DragScalar("##w", ImGuiDataType_U16, &selection->transform.w, 0.25f);
                ImGui::SameLine();
                ImGui::DragScalar("##h", ImGuiDataType_U16, &selection->transform.h, 0.25f);
                ImGui::PopItemWidth();

                ImGui::EndTable();
            }
        if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen))
            if (ImGui::BeginTable("##parameters_table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders))
            {
                ImGui::TableSetupColumn("Parameter");
                ImGui::TableSetupColumn("Value");
                ImGui::TableHeadersRow();

                for (auto& property : selection->properties)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted(property.display_name.c_str());
                    ImGui::TableNextColumn();
                    ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                    ImGui::InputScalar(("##" + property.display_name).c_str(), property.type, &property.value, &step);
                }
                ImGui::EndTable();
            }

        ImGui::EndChild();
        ImGui::End();
    }

    void ApplicationUI::draw_fix_env_popup()
    {
        const EnvironmentPath& cwd = std::filesystem::current_path();
        Environment& env = app->env;
        EnvironmentPath& dump = env.paths[EnvironmentPaths_Dump];
        // using this as a check to see if it's a valid dump folder (eventually should read files)
        EnvironmentPath& root = env.paths[EnvironmentPaths_Root];

        if (!open_fix_env_popup)
            ImGui::CloseCurrentPopup();

        // shown when the user needs to fix their environment variables due to a invalid result at boot
        if (ImGui::BeginPopupModal("Fix Environment Variables###fix_env_popup", 0, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("BlueShell has detected an invalid 'New! Super Mario Bros.' dump folder in its settings.\nPlease choose a valid 'New! Super Mario Bros.' dump folder.");
            ImGui::NewLine();

            ImGui::TextColored
            (
                { 0.85f, 0.25f, 0.25f, 1.0f },
                "Invalid dump folder: '%s'",
                dump.string().c_str()
            );
            if (dir_browser.HasSelected())
                ImGui::TextColored
                (
                    { 1.0f, 85.0f, 1.0f, 1.0f },
                    "New dump folder: '%s'",
                    dir_browser.GetSelected().string().c_str()
                );

            ImGui::Separator();

            if (ImGui::Button("Browse for a dump folder..."))
                dir_browser.Open();
            ImGui::SameLine();
            if (dir_browser.HasSelected())
                if (ImGui::ButtonEx("Next..."))
                {
                    const EnvironmentPath& p_dump = dir_browser.GetSelected();
                    // cheap, just checking if the "root" folder exists, easily spoofable
                    if (!std::filesystem::exists(p_dump / root))
                        ImGui::OpenPopup("###invalid-selection");
                    else
                        dump = p_dump;
                        ImGui::OpenPopup("###valid-selection");
                }
            if (ImGui::BeginPopupModal("Invalid Selection###invalid-selection", 0, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text
                (
                    "'%s' does not appear to be a valid 'New! Super Mario Bros.' dump.\nPlease select a valid 'New! Super Mario Bros.' dump.",
                    dir_browser.GetSelected().string().c_str()
                );
                ImGui::Separator();
                if (ImGui::Button("OK"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            if (ImGui::BeginPopupModal("Selection Saved###valid-selection", 0, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text
                (
                    "'%s' will be used as the default dump folder.",
                    dir_browser.GetSelected().string().c_str()
                );
                ImGui::Separator();
                if (ImGui::Button("OK"))
                {
                    env.save_paths_to_json();
                    env.save_jsons_to_disk(cwd);
                    
                    Debug::log("loading overlays and fnt.bin");
                    app->load_overlays();
                    app->load_filenames(0xF000);

                    open_fix_env_popup = false;
                    open_level_select_popup = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            dir_browser.Display();
            ImGui::EndPopup();
        }
    }

    void ApplicationUI::draw_level_select_popup()
    {
        // displays a list of all levels in the game organized by name
        // in a tree view. items are selectable and update Application::curr_area to selected level, area 0
        if (ImGui::BeginPopupModal("Select a Level to Open###level_select_popup", 0, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::BeginChild("level_listing_child", { 275, 500 }, ImGuiChildFlags_Border);
            for (auto& world : app->level_names)
                if (ImGui::TreeNodeEx(world.name.c_str(), ImGuiTreeNodeFlags_Framed))
                {
                    for (auto& level : world.levels)
                        if (ImGui::Selectable(("> " + level.name).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
                        {
                            app->curr_level_source = level;
                            Debug::log("loading " + level.name + ", area 1");
                            app->change_current_area(0);
                            relink_widgets_to_area(&app->curr_area);
                            
                            open_level_select_popup = false;
                            ImGui::CloseCurrentPopup();
                        }
                    ImGui::Separator();
                    ImGui::NewLine();
                    ImGui::TreePop();
                }
            ImGui::EndChild();
            ImGui::EndPopup();
        }
    }

    void ApplicationUI::relink_widgets_to_area(NSMB::Area* area)
    {
        for (sz_t i = 0; i < 3; i++)
        {
            object_picker_widgets.at(i).tileset = &area->tilesets.at(i);
            object_picker_widgets.at(i).populate();
        }
        area_editor.selection.clear();
        area_editor.area = area;
    }
}