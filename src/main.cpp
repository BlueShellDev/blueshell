// 
// BlueShell, a New! Super Mario Bros. level editor
// source v0.2.4
//

// editor
#include "editor.h"

Editor::Application      editor;
Editor::ApplicationUI    ui;

//for (sz_t i = 0; i < bit_width; i++)
//{
//    u8 bit = IO::read_bit(bytes, at);
//    value |= ((sz_t)bit << i);
//}

void main_loop()
{
    while (!glfwWindowShouldClose(ui.main_window))
    {
        ui.new_frame(); // must be called first every frame
        Render::bind_buffers_to_gpu(&editor.render_manager);

        ui.init_dock_space();
        ui.draw_main_menu_bar();
        ui.draw_selection_window();

        // popup logic
        if (ui.open_setup_popup)
            ImGui::OpenPopup("###setup_env_popup");
        if (ui.open_fix_env_popup)
            ImGui::OpenPopup("###fix_env_popup");
        if (ui.open_level_select_popup)
            ImGui::OpenPopup("###level_select_popup");
        ui.draw_fix_env_popup(editor.env);
        ui.draw_level_select_popup(editor);

        // area editor window
        ImGui::Begin("AreaEditorWindow");
        if (ui.area_editor.area)
            ui.area_editor.draw(&editor.render_manager, { 256, 256 });
        ImGui::End();

        // toolbox window
        ImGui::Begin("ToolboxWindow");
        ImGui::BeginTabBar("###toolbox_tabs");
        if (ImGui::BeginTabItem("Objects"))
        {
            ImGui::BeginTabBar("###tileset_tabs");
            if (ui.area_editor.area)
                for (auto& picker_widget : ui.object_picker_widgets)
                    if (ImGui::BeginTabItem(picker_widget.display_name.c_str()))
                    {
                        ui.area_editor.paint_item = picker_widget.selection;
                        picker_widget.draw(&editor.render_manager, { 50, 100 });
                        ImGui::EndTabItem();
                    }
            ImGui::EndTabBar();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
        ImGui::End();

        // status bar
        ImGui::Begin("StatusWindow", 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
        ImGui::Text(std::format("Zoom: {}x", ui.area_editor.draw_env.scale).c_str());
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
        if (ui.area_editor.area)
            ImGui::Text
            (
                std::format("Area {} of {} (PgUp/PgDown)", 
                ui.area_editor.area->area_index + 1, 
                editor.curr_level_source.areas.size()).c_str()
            );
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
        ImGui::Text(std::format("{} FPS", (int)ImGui::GetIO().Framerate).c_str());
        ImGui::End();

        if (ImGui::IsKeyPressed(ImGuiKey_PageUp))
        {
            editor.change_current_area(editor.curr_area.area_index + 1);
            ui.relink_widgets_to_area(&editor.curr_area);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_PageDown))
        {
            editor.change_current_area(editor.curr_area.area_index - 1);
            ui.relink_widgets_to_area(&editor.curr_area);
        }

        // end of frame stuff
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(ui.main_window);
        glfwPollEvents();
    }
}

int main()
{
    // program boot time, set some states up
    Debug::log("BlueShell now booting...");
    std::filesystem::path working_dir = std::filesystem::current_path();

    // first things first, initialize main window/render manager. if we fail here, just abort
    try
    {  
        // init ui main window
        Debug::log("initializing main window...");
        ui.init_main_window("BlueShell v0.2.4");

        // init backend renderer
        Debug::log("initializing render manager...");
        Editor::init_render_mgr(editor.render_manager, 9);
    }
    catch (const std::runtime_error& e)
    {
        Debug::log(e.what());
        return EXIT_FAILURE;
    }

    // ok we made it, setup look and feel
    Editor::set_editor_style();

    // now, the next thing that we should do is check for the existance of the "settings" & "res" directories (they should exist
    // in the same level as the executable). without them BlueShell cannot continue, so just quit with a message 
    // if we can't validate_paths their existence.

    Debug::log("checking for '" + editor.env.settings_dir.string() + "' directory...");
    if (!std::filesystem::exists(working_dir / editor.env.settings_dir))
    {
        // TODO: make this more helpful e.g direct the user on what to do from here.
        Debug::log("'" + editor.env.settings_dir.string() + "' directory could not be found... quitting");
        return EXIT_FAILURE;
    }
    Debug::log("'" + editor.env.settings_dir.string() + "' directory found, continuing to boot...");

    // if we have a valid settings dir, we can check for an 'settings/environment.json' file, which holds 
    // system paths as well as the rom directory layout (as needed by BlueShell). 
    // 
    // however, if we can't find an 'environment.json' file, instead of just quitting,
    // we will set a state that, when we enter main ui loop, will trigger a popup describing this issue, and should provide the 
    // user with the option to create a new 'environment.json' file.
    
    std::filesystem::path env_file = working_dir / editor.env.env_file;
    Debug::log("checking for '" + env_file.filename().string() + "'...");
    ui.open_setup_popup = !std::filesystem::exists(env_file);
    if (ui.open_setup_popup)
    {
        Debug::log("'" + env_file.filename().string() + "' could not be found... will ask user to create a new one");
    }
    else
    {
        Debug::log("'" + env_file.filename().string() + "' found...");

        // if we've entered this state, load the file into the Editor::Enviroment structure

        std::ifstream in;
        in = Util::begin_ifstream(env_file);
        editor.env.env_json = nlohmann::json::parse(in);
        in.close();

        // if we got here, we loaded the json into env. now we need to copy the json fields to the env.

        Debug::log("copying '" + env_file.filename().string() + "' into environment varaibles...");
        editor.env.copy_paths_from_json();
        // now validate_paths
        if (!editor.env.validate_paths())
        {
            // if the environment is invalid then set a state to open a popup when we enter the main loop to fix it.
            Debug::log("problems encountered while validating environment... will ask user to check environment varaibles");
            ui.open_fix_env_popup = true;
        }
    }
    
    // now we can just quick and dirtily check if any required resources can't be found. 
    // if they can't just spit out what it's looking for and quit. 

    Debug::log("checking for required resources in '" + editor.env.resources_dir.string() + "'...");
    std::ifstream fstream;
    try
    {
        // TODO: don't hardcode these filenames, put them in the environment
        fstream = Util::begin_ifstream(working_dir / editor.env.resources_dir / "overrides.json");
        editor.render_manager.overrides_json = nlohmann::json::parse(fstream); // TODO: figure out what the fuck is going on here (the overrides)
        fstream = Util::begin_ifstream(working_dir / editor.env.resources_dir / "tilesets.json");
        editor.env.tilesets_json = nlohmann::json::parse(fstream);
        fstream = Util::begin_ifstream(working_dir / editor.env.resources_dir / "worlds.json");
        editor.env.worlds_json = nlohmann::json::parse(fstream);
    }
    catch (const std::runtime_error& e)
    {
        Debug::log(e.what());
        return EXIT_FAILURE;
    }
    fstream.close();
    Debug::log("all required resources in '" + editor.env.resources_dir.string() + "' found, continuing to boot...");

    // populate name tables from jsons
    Debug::log("loading name tables from JSONs...");
    editor.load_names_from_json();  // rather weird, it should really have a json as a param instead of using one embedded in the env
                                    // quite confusing that way imo
    
    // ok, this is really all we need to do for sure on boot. the rest is state determined
    // btw, at this point, we can be in states: <!env_file_found> || <env_file_found && (fix_env_issues | !fix_env_issues)>

    // finally, before the main loop, we need to load into memory game-related files. which, obviously, 
    // can only be done if there are is an environment or !needs_fixing because they must be loaded from directories
    // specified there. 

    if (!ui.open_setup_popup && !ui.open_fix_env_popup)
    {
        // if we're in here, that means we can right away load overlay 0 & 10, the rom's filename table, and a nsbtx file for testing purposes.

        Debug::log("loading required ROM files...");
        try
        {
            editor.load_overlays();
            editor.load_filename_lookup(0xF000);
        }
        catch (const std::runtime_error& e)
        {
            Debug::log(e.what());
            return EXIT_FAILURE;
        }
        Debug::log("required ROM files loaded");
    }

    Debug::log("initialization complete, now entering main loop...");
    ui.open_level_select_popup = true;
    main_loop();
    glfwTerminate();
    Debug::log("program ended sucessfully");

    return 0;
}