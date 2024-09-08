#ifndef SETTINGS_H
#define SETTINGS_H
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> // Implicitly imports vulkan
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h> // For VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>

using std::array;
using std::set;
using std::string;
using std::vector;

using time_point = std::chrono::steady_clock::time_point;

#define DEF auto // Only use for the auto in functions, not for normal code so its easier to search for functions

// the do...while(0) is a hack to make it work everywhere, for example with if statements without braces without
extern size_t initVulkanIteration;
#define VULKAN_SETUP(func)                                                                  \
    do {                                                                                    \
        fprintf(stdout, " \033[32m(%zu.) initVulkan Step:\033[0m ", ++initVulkanIteration); \
        func();                                                                             \
    } while (0)

#define PRINT_BOLD_GREEN(text) fprintf(stdout, "\033[1m\033[32m\n%s\n\033[0m", text)

constexpr unsigned long long NO_TIMEOUT = UINT64_MAX; // Can't disable timeout in vulcan semaphore, this is a workaround for that
constexpr int INVALID_FRAMEBUFFER_SIZE = 0;

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    vector<VkSurfaceFormatKHR> formats;
    vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentationFamily;

    DEF isComplete() const -> bool {
        return graphicsFamily.has_value() && presentationFamily.has_value();
    }
};
struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static DEF getBindingDescription() -> VkVertexInputBindingDescription {
        VkVertexInputBindingDescription bindingDescription{
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
        return bindingDescription;
    }
    static DEF getAttributeDescriptions() -> array<VkVertexInputAttributeDescription, 2> {
        VkVertexInputAttributeDescription positionAttribute{
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Vertex, pos)};
        VkVertexInputAttributeDescription colorAttribute{
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, color)};
        return {positionAttribute, colorAttribute};
    }
};
/* Vertices w/ counter-clockwise winding order (TODO: Update graphic with vertices from book)
  2: (-0.5, 0.5)     3: (0.5, 0.5)
     *-----<<<-----*
     |          __/|
     v       __/   |
     v    >>^      ^
     | __/         ^
     |/            |
     *----->>>-----*
  1: (-0.5, -0.5)    0: (0.5, -0.5)
*/
// clang-format off
const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};
const vector<uint16_t> indices = {0, 1, 2, 2, 3, 0}; // Set to uint32_t if we get too many vertices
// clang-format on

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

namespace FilePaths {
constexpr const char *SHADER_VERT = "/Users/danielsinkin/GitHub_private/Vulkan/shaders/compiled/vert.spv";
constexpr const char *SHADER_FRAG = "/Users/danielsinkin/GitHub_private/Vulkan/shaders/compiled/frag.spv";
} // namespace FilePaths

namespace Util {
static DEF readFile(const string &filename) -> vector<char> {
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

    vector<char> buffer(fileSize);
    file.seekg(0);

    // Cast fileSize to std::streamsize after checking the range
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
    file.close();

    return buffer;
}

} // namespace Util

namespace Settings {
constexpr uint32_t DEFAULT_WINDOW_WIDTH = 800;
constexpr uint32_t DEFAULT_WINDOW_HEIGHT = 600;

constexpr auto WINDOW_NAME = "Vulkan 3D Engine";

// For example macbooks have integrated graphics cards, so they would be filtered by this, which wouldn't make sense
constexpr bool ALLOW_DEVICE_WITHOUT_INTEGRATED_GPU = true;
constexpr bool ALLOW_DEVICE_WITHOUT_GEOMETRY_SHADER = true;

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

constexpr float CLIPPING_PLANE_NEAR = 0.1f;
constexpr float CLIPPING_PLANE_FAR = 10.0f;

// Define the preferred surface format as a VkSurfaceFormatKHR struct
constexpr VkSurfaceFormatKHR PREFERRED_SURFACE_FORMAT = {
    VK_FORMAT_B8G8R8A8_SRGB,          // format
    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR // colorSpace
};

} // namespace Settings
#endif // SETTINGS_H