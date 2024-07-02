cmake_minimum_required (VERSION 3.8)

project(
    BlueShellLevelEditor
    VERSION 0.2.1
)

find_package(glfw3 REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(imgui REQUIRED)

add_executable(BlueShellLevelEditor)

target_include_directories(
    BlueShellLevelEditor
    PRIVATE
    ${CMAKE_SOURCE_DIR}/
)

if (WIN32)
    target_link_libraries(
        BlueShellLevelEditor
        PRIVATE
        glfw 
        opengl32.lib 
        imgui::imgui
    )
else()
    target_link_libraries(
        BlueShellLevelEditor
        PRIVATE
        glfw 
        OpenGL
        imgui::imgui
    )
endif()

target_sources(
    BlueShellLevelEditor
    PRIVATE
    ${CMAKE_SOURCE_DIR}/BlueShellLevelEditor.cpp
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET BlueShellLevelEditor PROPERTY CXX_STANDARD 20)
endif()
