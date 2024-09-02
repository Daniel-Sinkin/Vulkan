#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> // Implicitly imports vulcan

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

// Initial Window size
const uint32_t WINDOW_WIDTH = 800;
const uint32_t WINDOW_HEIGHT = 600;

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow *window;
    VkInstance instance;

    void initWindow() {
        glfwInit();

        // GLFW defaults to creating OpenGL context, this suppresses that
        glfwWindowHint(GLFW_CLIENT_API, GLFW_FALSE);
        // Disables window resizing
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        // 4th arg is if we want to specify a particular window, uses main window if nullptr is used.
        // Last argument is OpenGL specific, therefore irrelevant for us
        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan", nullptr, nullptr);
    }

    bool const validateExtensions(const std::vector<VkExtensionProperties> &supported_extensions, const char **required_extensions, const uint32_t required_extensions_count) {
        for (uint32_t i = 0; i < required_extensions_count; i++) {
            bool is_included = false;
            for (const auto &required_extension : supported_extensions) {
                if (std::strcmp(required_extensions[i], required_extension.extensionName)) {
                    is_included = true;
                    break;
                }
            }
            if (!is_included) {
                return false;
            }
        }
        return true;
    }

    void createInstance() {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        if (glfwExtensions) {
            for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
                std::cout << glfwExtensions[i] << std::endl;
            }
        } else {
            std::cout << "No required extensions found." << std::endl;
        }
        std::cout << "\n";

        // Fix for VK_ERROR_INCOMPATIBLE_DRIVER that appears in MacOS
        std::vector<const char *> requiredExtensions;
        for (uint32_t i = 0; i < glfwExtensionCount; i++) {
            requiredExtensions.emplace_back(glfwExtensions[i]);
        }
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.pNext = nullptr;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);

        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::cout << "available extensions:\n";

        for (const auto &extension : extensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }

        if (!validateExtensions(extensions, glfwExtensions, glfwExtensionCount)) {
            std::cout << "Not Supported!\n";
        }
    }

    void initVulkan() {
        createInstance();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    // Our application
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}