cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(
    BlueShellLevelEditor
    VERSION 0.2.1
)

find_package(glfw3 REQUIRED)
find_package(glew REQUIRED)
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
        GLEW::glew 
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
