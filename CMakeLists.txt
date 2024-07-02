cmake_minimum_required (VERSION 3.8)

project(
    BlueShellLevelEditor
    VERSION 0.2.1
)

find_package(glfw3 REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(imgui REQUIRED)

add_library(nlohmann-json INTERFACE)
target_include_directories(nlohmann-json INTERFACE deps/nlohmann)

add_library(imfilebrowser INTERFACE)
target_include_directories(imfilebrowser INTERFACE deps/imfilebrowser)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE deps/stb)

add_custom_target(res
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/res
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res
        COMMENT "copying ${CMAKE_SOURCE_DIR}/res to ${CMAKE_BINARY_DIR}/res"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )

file(GLOB_RECURSE SOURCE src/*.cpp src/*.hpp src/*.h)

add_executable(BlueShellLevelEditor ${SOURCE})
add_dependencies(BlueShellLevelEditor res)
target_link_libraries(BlueShellLevelEditor PRIVATE nlohmann-json imfilebrowser stb)

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

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET BlueShellLevelEditor PROPERTY CXX_STANDARD 20)
endif()
