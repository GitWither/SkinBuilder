#include <iostream>
#include <GLFW/glfw3.h>
#include "glm/vec2.hpp"
#include <vulkan/vulkan.h>

int main() {

    if (!glfwInit())
    {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Skin Builder", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);


    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
}