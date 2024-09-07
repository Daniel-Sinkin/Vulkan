#ifndef SETTINGS_H
#define SETTINGS_H
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>         // Implicitly imports vulkan
#include <vulkan/vulkan_beta.h> // For VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <string_view>
#include <vector>

using std::set;
using std::string;
using std::vector;

inline bool operator==(const VkSurfaceFormatKHR &lhs, const VkSurfaceFormatKHR &rhs) {
    return lhs.format == rhs.format && lhs.colorSpace == rhs.colorSpace;
}

namespace Util {
static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}
} // namespace Util

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