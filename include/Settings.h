#ifndef SETTINGS_H
#define SETTINGS_H
#include <GLFW/glfw3.h>         // Implicitly imports vulkan
#include <vulkan/vulkan_beta.h> // For VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME

inline bool operator==(const VkSurfaceFormatKHR &lhs, const VkSurfaceFormatKHR &rhs) {
    return lhs.format == rhs.format && lhs.colorSpace == rhs.colorSpace;
}

namespace Settings {
// For example macbooks have integrated graphics cards, so they would be filtered by this, which wouldn't make sense
constexpr bool ALLOW_DEVICE_WITHOUT_INTEGRATED_GPU = true;

// Define the preferred surface format as a VkSurfaceFormatKHR struct
constexpr VkSurfaceFormatKHR PREFERRED_SURFACE_FORMAT = {
    VK_FORMAT_B8G8R8A8_SRGB,          // format
    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR // colorSpace
};

} // namespace Settings
#endif // SETTINGS_H