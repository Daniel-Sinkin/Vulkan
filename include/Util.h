#ifndef UTIL_H
#define UTIL_H

#include "Constants.h"

#define VULKAN_SETUP(func)                                                                       \
    {                                                                                            \
        fprintf(stdout, " \033[32m(%zu.) initVulkan Step:\033[0m ", ++initVulkanIteration);      \
        fprintf(stdout, "Trying to initialize %s\n", #func);                                     \
        auto start = std::chrono::high_resolution_clock::now();                                  \
        func();                                                                                  \
        auto end = std::chrono::high_resolution_clock::now();                                    \
        std::chrono::duration<double, std::milli> elapsed = end - start;                         \
        fprintf(stdout, "Successfully initialized %s (Took %.2f ms)\n", #func, elapsed.count()); \
    }

#define TIMED_EXECUTION(func)                                            \
    {                                                                    \
        auto start = std::chrono::high_resolution_clock::now();          \
        func();                                                          \
        auto end = std::chrono::high_resolution_clock::now();            \
        std::chrono::duration<double, std::milli> elapsed = end - start; \
        fprintf(stdout, "%s took %.2f ms\n", #func, elapsed.count());    \
    }

#define PRINT_BOLD_GREEN(text) fprintf(stdout, "\033[1m\033[32m\n%s\n\033[0m", text)

// Overloaded glmPrint for mat4
inline void glmPrint(const glm::mat4 &mat) {
    std::cout << "mat4(" << std::endl;
    for (int i = 0; i < 4; ++i) {
        std::cout << "  " << mat[i][0] << ", " << mat[i][1] << ", " << mat[i][2] << ", " << mat[i][3] << std::endl;
    }
    std::cout << ")" << std::endl;
}

// Overloaded glmPrint for vec2
inline void glmPrint(const glm::vec2 &vec) {
    std::cout << "vec2(" << vec.x << ", " << vec.y << ")" << std::endl;
}

// Overloaded glmPrint for vec3
inline void glmPrint(const glm::vec3 &vec) {
    std::cout << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
}

// Overloaded glmPrint for vec4
inline void glmPrint(const glm::vec4 &vec) {
    std::cout << "vec4(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")" << std::endl;
}

// Overloaded glmPrint for mat3
inline void glmPrint(const glm::mat3 &mat) {
    std::cout << "mat3(" << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << "  " << mat[i][0] << ", " << mat[i][1] << ", " << mat[i][2] << std::endl;
    }
    std::cout << ")" << std::endl;
}

#endif // CONSTANTS_H

/*
// clang-format off
std::string vkResultToString(VkResult result) {
    switch (result) {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case VK_ERROR_FRAGMENTATION_EXT: return "VK_ERROR_FRAGMENTATION_EXT";
        case VK_ERROR_NOT_PERMITTED_EXT: return "VK_ERROR_NOT_PERMITTED_EXT";
        default: return "Unknown Vulkan error";
    }
}
// clang-format on

template <typename VulkanFunction, typename... Args>
inline void vkCreationWrapper(const std::string &operationName, VulkanFunction func, Args &&...args) {
    VkResult result = func(std::forward<Args>(args)...);
    if (result != VK_SUCCESS) {
        std::string errorName = vkResultToString(result);
        std::stringstream ss;
        ss << "\nFailed to create " << operationName << "! <Error Name: " << errorName << ", Error Code: " << result << ">\n";
        throw std::runtime_error(ss.str());
    }
}
*/