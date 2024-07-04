cmake_minimum_required (VERSION 3.12)

project(
    BlueShell
    VERSION 0.2.3
)

find_package(glfw3 REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(imgui REQUIRED)
find_package(nlohmann_json REQUIRED)

if(APPLE)
find_package(glew REQUIRED)
endif(APPLE)

add_library(imfilebrowser INTERFACE)
target_include_directories(imfilebrowser INTERFACE ${CMAKE_SOURCE_DIR}/deps/imfilebrowser)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_SOURCE_DIR}/deps/stb)

add_custom_target(
    res
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/res
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/settings
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res
    COMMENT "copying ${CMAKE_SOURCE_DIR}'/res' to ${CMAKE_BINARY_DIR}'/res'"
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/settings ${CMAKE_BINARY_DIR}/settings
    COMMENT "copying ${CMAKE_SOURCE_DIR}'/settings' to ${CMAKE_BINARY_DIR}'/settings'"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

file(
    GLOB_RECURSE 
    SOURCE 
    ${CMAKE_SOURCE_DIR}/src/*.cpp 
    ${CMAKE_SOURCE_DIR}/src/*.hpp 
    ${CMAKE_SOURCE_DIR}/src/*.h
)

add_executable(${PROJECT_NAME} ${SOURCE})
add_dependencies(${PROJECT_NAME} res)

target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE 
    nlohmann_json::nlohmann_json
    imfilebrowser 
    stb
)

if (WIN32)
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        glfw 
        opengl32.lib 
        imgui::imgui
    )
elseif(APPLE)
    target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	glfw
	GLEW::GLEW
	imgui::imgui
    )
else()
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        glfw 
        OpenGL
        imgui::imgui
    )
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
