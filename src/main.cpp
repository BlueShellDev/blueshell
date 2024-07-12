// 
// BlueShell, a New! Super Mario Bros. level editor
//

// editor
#include "editor.h"

Editor::Application      editor;
Editor::ApplicationUI    ui;
Editor::DiscordIntegration  discord_integration;

void main_loop()
{
    while (!glfwWindowShouldClose(ui.main_window))
    {
        ui.new_frame(); // must be called first every frame
        Render::bind_buffers_to_gpu(&editor.render_manager);

        ui.init_dock_space();

        // popup logic
        if (ui.open_fix_env_popup){
            discord_integration.activity.SetDetails("Picking a ROM dump folder...");
            discord_integration.core->ActivityManager().UpdateActivity(discord_integration.activity, [](discord::Result result){});
            ImGui::OpenPopup("###fix_env_popup");
        }
        if (ui.open_level_select_popup){
            discord_integration.activity.SetDetails("Choosing a level");
            discord_integration.activity.SetState("");
            discord_integration.core->ActivityManager().UpdateActivity(discord_integration.activity, [](discord::Result result){});
            ImGui::OpenPopup("###level_select_popup");
        }

        try
        {
            ui.draw_fix_env_popup();
            ui.draw_level_select_popup();
            ui.draw_main_menu_bar();
            ui.draw_area_editor_window({ 512, 256 });
            ui.draw_toolbox_window();
            ui.draw_status_window();
            ui.draw_selection_window();
        
            if (ImGui::IsKeyDown(ImGuiMod_Shift) && ImGui::IsKeyPressed(ImGuiKey_UpArrow))
            {
                editor.change_current_area(editor.curr_area.area_index + 1);
                ui.relink_widgets_to_area(&editor.curr_area);
                discord_integration.activity.SetDetails("Editing a level");
                discord_integration.activity.SetState(editor.curr_level_source.name.c_str());
                discord_integration.core->ActivityManager().UpdateActivity(discord_integration.activity, [](discord::Result result){});
            }
            if (ImGui::IsKeyDown(ImGuiMod_Shift) && ImGui::IsKeyPressed(ImGuiKey_DownArrow))
            {
                editor.change_current_area(editor.curr_area.area_index - 1);
                ui.relink_widgets_to_area(&editor.curr_area);
                discord_integration.activity.SetDetails("Editing a level");
                discord_integration.activity.SetState(editor.curr_level_source.name.c_str());
                discord_integration.core->ActivityManager().UpdateActivity(discord_integration.activity, [](discord::Result result){});
            }
        }
        catch (const nlohmann::json::parse_error& e)
        {
            Debug::log(e.what());
        }
        catch (const std::runtime_error& e)
        {
            Debug::log(e.what());
        }
        catch (const std::out_of_range& e)
        {
            Debug::log(e.what());
        }

        // end of frame stuff
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(ui.main_window);
        glfwPollEvents();

        ::discord_integration.core->RunCallbacks();
    }
}

int main()
{
    // program boot time, set some states up
    Debug::log("BlueShell booting...");

    // helpful refs
    const Editor::EnvironmentPath& cwd = std::filesystem::current_path();
    Editor::ApplicationJSON& env_json = editor.env.jsons[Editor::ApplicationJSON_Environment];

    // first things first, initialize main window/render manager. if we fail here, just abort
    try
    {  
        // init ui main window
        Debug::log("initializing main window...");
        ui.init_main_window("BlueShell [NSMB (DS) Level Editor]");

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

    // Initalize Discord activity status
    discord::Core::Create(1260663285988982837, DiscordCreateFlags_Default, &discord_integration.core);
    discord_integration.core->ActivityManager().UpdateActivity(discord_integration.activity, [](discord::Result result){});

    // load program jsons
    Debug::log("loading json files...");
    try
    {
        for (auto& app_json : editor.env.jsons)
        {
            const Editor::EnvironmentPath& real_path = cwd / app_json.first;
            app_json.second = Util::load_json_from_disk(real_path);
        }
    }
    catch (const std::runtime_error& e)
    {
        Debug::log(e.what());
        return EXIT_FAILURE;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        Debug::log(e.what());
        return EXIT_FAILURE;
    }
    Debug::log("json files loaded... ");

    // jsons should be loaded here, so we can update paths 
    editor.load_names_from_env(); // move method and sources to env? 
    editor.env.load_paths_from_json();  
    const Editor::EnvironmentPath& dump = editor.env.paths[Editor::EnvironmentPaths_Dump];
    const Editor::EnvironmentPath& root = editor.env.paths[Editor::EnvironmentPaths_Root];
    // open a fixer popup if we can't find a root folder within a dump
    ui.open_fix_env_popup = !std::filesystem::exists(dump / root);
    
    if (!ui.open_fix_env_popup)
    {
        // load some game dependancies (can only do this if we're sure the env paths are good)
        try
        {
            editor.load_overlays();
            editor.load_filenames(0xF000);
        }
        catch (const std::runtime_error& e)
        {
            Debug::log(e.what());
            return EXIT_FAILURE;
        }
    }
    else
        Debug::log("path errors detected... should ask the user to review");

    Debug::log("entering main loop...");
    // link ui layer to editor
    ui.app = &editor;
    main_loop();

    glfwTerminate();
    Debug::log("quit sucessfully");
    return 0;
}