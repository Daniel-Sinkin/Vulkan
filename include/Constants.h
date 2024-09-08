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
#include <numbers>
#include <optional>
#include <set>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>

using glm::mat4;
using glm::vec2;
using glm::vec3;

using std::array;
using std::set;
using std::string;
using std::vector;

using time_point = std::chrono::steady_clock::time_point;

#define DEF auto // Only use for the auto in functions, not for normal code so its easier to search for functions

// the scoping is a hack to make it work everywhere, for example with "if statements" without braces
#define VULKAN_SETUP(func)                                                                  \
    {                                                                                       \
        fprintf(stdout, " \033[32m(%zu.) initVulkan Step:\033[0m ", ++initVulkanIteration); \
        func();                                                                             \
    }

#define PRINT_BOLD_GREEN(text) fprintf(stdout, "\033[1m\033[32m\n%s\n\033[0m", text)

constexpr unsigned long long NO_TIMEOUT = UINT64_MAX; // Can't disable timeout in vulcan semaphore, this is a workaround for that
constexpr int INVALID_FRAMEBUFFER_SIZE = 0;

// numbers::pi is much more accurate than numbers::pi_v
constexpr float PI = std::numbers::pi_v<float>;
constexpr float PI_2 = static_cast<float>(2.0 * std::numbers::pi);
constexpr float PI_HALF = static_cast<float>(std::numbers::pi / 2);
constexpr float PI_QUARTER = static_cast<float>(std::numbers::pi / 4);

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
    vec2 pos;
    vec3 color;

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
  3: (-0.5, 0.5)     2: (0.5, 0.5)
     *-----<<<-----*
     |          __/|
     v       __/   |
     v    >>^      ^
     | __/         ^
     |/            |
     *----->>>-----*
  0: (-0.5, -0.5)    1: (0.5, -0.5)
*/
// clang-format off
const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}},
    {{ 0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}},
    {{ 0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}},
    {{-0.5f,  0.5f}, { 1.0f,  1.0f,  1.0f}}
};
const vector<uint16_t> vertexIndices = {0, 1, 2, 2, 3, 0}; // Set to uint32_t if we get too many vertices

// These are badly setup and won't draw unless backface culling is completely disabled
const std::vector<Vertex> vertices_Hexagon = {
    { {  0.0000f,  0.0000f }, { 1.0f, 1.0f, 1.0f } },
    { {  0.0000f,  1.0000f }, { 1.0f, 1.0f, 1.0f } },
    { {  0.8660f,  0.5000f }, { 1.0f, 1.0f, 1.0f } },
    { {  0.8660f, -0.5000f }, { 1.0f, 1.0f, 1.0f } },
    { {  0.0000f, -1.0000f }, { 1.0f, 1.0f, 1.0f } },
    { { -0.8660f, -0.5000f }, { 1.0f, 1.0f, 1.0f } },
    { { -0.8660f,  0.5000f }, { 1.0f, 1.0f, 1.0f } },
    { {  0.0000f,  0.5000f }, { 1.0f, 1.0f, 1.0f } },
    { {  0.4330f,  0.7500f }, { 1.0f, 1.0f, 1.0f } },
    { {  0.4330f,  0.2500f }, { 1.0f, 1.0f, 1.0f } },
    { {  0.8660f,  0.0000f }, { 1.0f, 1.0f, 1.0f } },
    { {  0.4330f, -0.2500f }, { 1.0f, 1.0f, 1.0f } },
    { {  0.4330f, -0.7500f }, { 1.0f, 1.0f, 1.0f } },
    { {  0.0000f, -0.5000f }, { 1.0f, 1.0f, 1.0f } },
    { { -0.4330f, -0.7500f }, { 1.0f, 1.0f, 1.0f } },
    { { -0.4330f, -0.2500f }, { 1.0f, 1.0f, 1.0f } },
    { { -0.8660f,  0.0000f }, { 1.0f, 1.0f, 1.0f } },
    { { -0.4330f,  0.2500f }, { 1.0f, 1.0f, 1.0f } },
    { { -0.4330f,  0.7500f }, { 1.0f, 1.0f, 1.0f } },
};

const std::vector<uint16_t> indices_Hexagon = {
    0, 7, 9,
    7, 1, 8,
    9, 8, 2,
    7, 8, 9,
    0, 9, 11,
    9, 2, 10,
    11, 10, 3,
    9, 10, 11,
    0, 11, 13,
    11, 3, 12,
    13, 12, 4,
    11, 12, 13,
    0, 13, 15,
    13, 4, 14,
    15, 14, 5,
    13, 14, 15,
    0, 15, 17,
    15, 5, 16,
    17, 16, 6,
    15, 16, 17,
    0, 17, 7,
    17, 6, 18,
    7, 18, 1,
    17, 18, 7,
};
// clang-format on

struct UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
};

namespace FilePaths {
constexpr const char *SHADER_VERT = "/Users/danielsinkin/GitHub_private/Vulkan/shaders/compiled/shader.vert.spv";
constexpr const char *SHADER_FRAG = "/Users/danielsinkin/GitHub_private/Vulkan/shaders/compiled/shader.frag.spv";
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

constexpr vec3 CAMERA_EYE(2.0f, 2.0f, 2.0f);
constexpr vec3 CAMERA_CENTER(0.0f, 0.0f, 0.0f);
constexpr vec3 CAMERA_UP(0.0f, 0.0f, 1.0f);

constexpr float CLIPPING_PLANE_NEAR = 0.1f;
constexpr float CLIPPING_PLANE_FAR = 10.0f;

// Define the preferred surface format as a VkSurfaceFormatKHR struct
constexpr VkSurfaceFormatKHR PREFERRED_SURFACE_FORMAT = {
    VK_FORMAT_B8G8R8A8_SRGB,          // format
    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR // colorSpace
};

} // namespace Settings

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
void vkCreationWrapper(const std::string &operationName, VulkanFunction func, Args &&...args) {
    VkResult result = func(std::forward<Args>(args)...);
    if (result != VK_SUCCESS) {
        std::string errorName = vkResultToString(result);
        std::stringstream ss;
        ss << "\nFailed to create " << operationName << "! <Error Name: " << errorName << ", Error Code: " << result << ">\n";
        throw std::runtime_error(ss.str());
    }
}

#endif // SETTINGS_H