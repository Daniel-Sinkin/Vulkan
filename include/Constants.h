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
        if (!std::filesystem::exists(filename)) {
            fprintf(stderr, "Error: File '%s' does not exist.\n", filename.c_str());
        } else if (!std::filesystem::is_regular_file(filename)) {
            fprintf(stderr, "Error: '%s' is not a regular file.\n", filename.c_str());
        } else {
            fprintf(stderr, "Error: Unable to open file '%s': %s\n", filename.c_str(), std::strerror(errno));
        }
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    if (fileSize > std::numeric_limits<std::streamsize>::max()) {
        throw std::runtime_error("File size exceeds the maximum supported size.");
    }

    std::vector<char> buffer(fileSize);
    file.seekg(0);

    // Cast fileSize to std::streamsize after checking the range
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
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