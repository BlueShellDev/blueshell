#pragma once
#include <string>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

namespace Core
{
    std::string GetGLSLVersion();
    GLFWwindow *MakeGLFWWindow(const char *title, int width, int height);
    void ClearGLFWWindowWithColor(GLFWwindow *window, float r, float g, float b, float a);
}