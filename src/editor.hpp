#pragma once

// opengl
#include "GL/glew.h"
#include "GLFW/glfw3.h"

// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"

// internal
#include "graphics/render.h"
#include "util/util.hpp"

namespace Editor
{
    struct Application
    {
        /* links to main app services */
        GLFWwindow* main_window = nullptr;

        /* handles pixbufs, lives here */
        Render::RenderManager   render_manager;

        /* program constants */
        const std::string       main_window_title = "BlueShell 0.2.1a";
        const int               default_main_window_width = 1920; // px
        const int               default_main_window_height = 1080; // px

        const std::string       font_path = "res/super-mario-ds-original.ttf";
        const std::string       pipe_override_path = "res/pipe_override_2.png";
        const std::string       jyotyu_override_path = "res/block_override.png";
        const std::string       override_json = "res/overrides.json";
        const std::string       tileset_json = "res/names.json";
        const std::string       worlds_json = "res/worlds.json";
     
        std::string       base_path = "D:/Documents/NDS Games/FILESYSTEM/root/";

        const std::string       ov0_path = "D:/Documents/NDS Games/FILESYSTEM/overlay9/overlay9_0.bin";
        const std::string       ov10_path = "D:/Documents/NDS Games/FILESYSTEM/overlay9/overlay9_10.bin";
        const std::string       fnt_path = "D:/Documents/NDS Games/FILESYSTEM/fnt.bin";

        const std::string       course_path = "course/";
        const std::string       ncg_dir = "BG_ncg/";
        const std::string       ncl_dir = "BG_ncl/";
        const std::string       pnl_dir = "BG_pnl/";
        const std::string       unt_dir = "BG_unt/";
        const std::string       nsbtx_dir = "polygon_unit/";

        const std::string       t0_ncg_path = "d_2d_A_J_jyotyu_ncg.bin";
        const std::string       t0_pnl_path = "d_2d_PA_A_J_jyotyu.bin";
        const std::string       t0_unt_path = "A_J_jyotyu.bin";

        const std::string       t2_ncg_path = "d_2d_I_S_tikei_nohara_ncg.bin";
        const std::string       t2_ncl_path = "d_2d_I_S_tikei_nohara_ncl.bin";
        const std::string       t2_pnl_path = "d_2d_PA_I_S_nohara.bin";
        const std::string       t2_unt_path = "I_S_nohara.bin";

        const std::string       nsbtx_path = "I_dokan.nsbtx";
    };

    static void initialize_main_window(Application* app)
    {
        /* basic boilerplate GLFW shit, just create a window and make sure it's there */
        if (!glfwInit()) 
            throw std::runtime_error("failed to initialize GLFW!");

        app->main_window = glfwCreateWindow
        (
            app->default_main_window_width,
            app->default_main_window_height,
            app->main_window_title.c_str(),
            nullptr,
            nullptr
        );

        glfwMaximizeWindow(app->main_window);

        if (!app->main_window)
        {
            glfwTerminate();
            throw std::runtime_error("failed to create the main window!");
        }

        glfwMakeContextCurrent(app->main_window);

        /* now, i'mgui */
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
        //ImFont* font0 = ImGui::GetIO().Fonts->AddFontFromFileTTF("D:\\Documents\\Hack-Regular.ttf", 16.0f);
        //ImFont* font1 = ImGui::GetIO().Fonts->AddFontFromFileTTF(app->font_path.c_str(), 13.0f);

        {
            ImVec4* colors = ImGui::GetStyle().Colors;
            colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
            colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.13f, 0.93f);
            colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.27f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.13f, 0.92f);
            colors[ImGuiCol_Border] = ImVec4(0.44f, 0.37f, 0.61f, 0.64f);
            colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.27f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.20f, 0.25f, 1.00f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
            colors[ImGuiCol_TitleBg] = ImVec4(0.33f, 0.28f, 0.47f, 0.64f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.50f, 0.42f, 0.69f, 0.64f);
            colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.32f, 0.27f, 0.44f, 0.64f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
            colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.13f, 1.00f);
            colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.19f, 0.20f, 0.25f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.24f, 0.24f, 0.32f, 1.00f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.74f, 0.58f, 0.98f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.44f, 0.37f, 0.61f, 0.54f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.58f, 0.98f, 0.54f);
            colors[ImGuiCol_Button] = ImVec4(0.30f, 0.26f, 0.43f, 0.94f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.60f, 0.50f, 0.83f, 0.94f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.60f, 0.45f, 0.99f, 0.94f);
            colors[ImGuiCol_Header] = ImVec4(0.44f, 0.37f, 0.61f, 0.29f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.50f, 0.42f, 0.69f, 0.29f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.45f, 0.75f, 0.29f);
            colors[ImGuiCol_Separator] = ImVec4(0.44f, 0.37f, 0.61f, 1.00f);
            colors[ImGuiCol_SeparatorHovered] = ImVec4(0.74f, 0.58f, 0.98f, 1.00f);
            colors[ImGuiCol_SeparatorActive] = ImVec4(0.84f, 0.58f, 1.00f, 1.00f);
            colors[ImGuiCol_ResizeGrip] = ImVec4(0.44f, 0.37f, 0.61f, 0.29f);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.74f, 0.58f, 0.98f, 0.29f);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(0.84f, 0.58f, 1.00f, 0.29f);
            colors[ImGuiCol_Tab] = ImVec4(0.30f, 0.26f, 0.43f, 0.64f);
            colors[ImGuiCol_TabHovered] = ImVec4(0.60f, 0.50f, 0.83f, 0.64f);
            colors[ImGuiCol_TabActive] = ImVec4(0.60f, 0.45f, 0.99f, 0.64f);
            colors[ImGuiCol_TabUnfocused] = ImVec4(0.19f, 0.16f, 0.26f, 0.64f);
            colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.60f, 0.45f, 0.99f, 0.64f);
            colors[ImGuiCol_DockingPreview] = ImVec4(0.44f, 0.37f, 0.61f, 1.00f);
            colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
            colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
            colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
            colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
            colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
            colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
            colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
            colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
            colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
            colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
            colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
            colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

            auto& style = ImGui::GetStyle();
            style.TabRounding = 4;
            style.ScrollbarRounding = 4;
            style.WindowRounding = 4;
            style.GrabRounding = 4;
            style.FrameRounding = 4;
            style.PopupRounding = 4;
            style.ChildRounding = 4;

            style.WindowBorderSize = 0;
            style.FrameBorderSize = 0;
            style.TabBarBorderSize = 1;

            style.FramePadding = { 6, 4 };
        }

        ImGui_ImplGlfw_InitForOpenGL(app->main_window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    static void initialize_renderer(Application* app)
    {
        /* setup the pixbufs */
        app->render_manager.number_of_textures_to_gen = 9;
        Render::initialize_opengl_textures(&app->render_manager);
        Render::initialize_pixmap_table(&app->render_manager);

        /* load override images as textures */
        Render::load_image_to_texture(&app->render_manager, &app->render_manager.overrides_texture, app->jyotyu_override_path);
        Render::load_image_to_texture(&app->render_manager, &app->render_manager.pipes_texture, app->pipe_override_path);
    }
}