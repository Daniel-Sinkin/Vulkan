#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class VulkanEngine {
public:
    void run();

private:
    GLFWwindow *window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;

    void initWindow();
    void initVulkan();
    void createInstance();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void mainLoop();
    void cleanup();
};

#endif // VULKAN_ENGINE_H
