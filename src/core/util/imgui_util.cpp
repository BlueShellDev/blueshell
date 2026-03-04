#include "core/util/imgui_util.h"
#include "imgui_util.h"

void BS::Util::BeginImGui(GLFWwindow *mainWindow, std::string glslVersion, bool supportDocking = true)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // imgui configs
    if (supportDocking)
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
    ImGui_ImplOpenGL3_Init(glslVersion.c_str());
}

void BS::Util::BeginImGuiFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void BS::Util::InitializeImGuiDockspace(std::string dockspaceID)
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = 0 |
                                    ImGuiWindowFlags_MenuBar |
                                    ImGuiWindowFlags_NoDocking |
                                    ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus |
                                    ImGuiWindowFlags_NoNavFocus;
    ImGui::Begin("Dockspace", NULL, window_flags);
    ImGuiID mainDockspaceID = ImGui::GetID(dockspaceID.c_str());
    ImGui::DockSpace(mainDockspaceID);
    ImGui::End();
}

void BS::Util::EndImGuiFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void BS::Util::EndImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
