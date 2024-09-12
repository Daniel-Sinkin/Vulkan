#ifndef CONSTANTS_H
#define CONSTANTS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> // Implicitly imports vulkan
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h> // For VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

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
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

using std::all_of;
using std::any_of;
using std::array;
using std::find_if;
using std::optional;
using std::runtime_error;
using std::set;
using std::string;
using std::string_view;
using std::unordered_map;
using std::vector;

// When using forward declaration we have an auto before the function name, to be able to seperate
// it from the normal auto we find in the code I've introduced this macro, inspired by the
// python syntax for functions, which makes it easier to search function definitions.
#define DEF auto

constexpr unsigned long long NO_TIMEOUT = UINT64_MAX; // Can't disable timeout in Vulkan semaphore, this is a workaround
constexpr int INVALID_FRAMEBUFFER_SIZE = 0;

// numbers::pi is much more accurate than numbers::pi_v
constexpr float PI = std::numbers::pi_v<float>;
constexpr float PI_2 = static_cast<float>(2.0 * std::numbers::pi); // Use static_cast for type conversion
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
    vec3 pos;
    vec3 color;
    vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};

        return bindingDescription;
    }

    static DEF getAttributeDescriptions() -> array<VkVertexInputAttributeDescription, 3> {
        return array<VkVertexInputAttributeDescription, 3>{
            VkVertexInputAttributeDescription{
                .binding = 0,
                .location = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = static_cast<uint32_t>(offsetof(Vertex, pos))},
            VkVertexInputAttributeDescription{
                .binding = 0,
                .location = 1,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = static_cast<uint32_t>(offsetof(Vertex, color))},
            VkVertexInputAttributeDescription{
                .binding = 0,
                .location = 2,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = static_cast<uint32_t>(offsetof(Vertex, texCoord))}};
    }

    DEF operator==(const Vertex &other) const->bool {
        return (this->pos == other.pos) && (this->color == other.color) && (this->texCoord == other.texCoord);
    }
};
namespace std {
// Implementing our hashing function into the stdlib hash template, for more details see https://en.cppreference.com/w/cpp/utility/hash
template <>
struct hash<Vertex> {
    size_t operator()(Vertex const &vertex) const {
        size_t pos_hash = hash<glm::vec3>()(vertex.pos);
        size_t color_hash = hash<glm::vec3>()(vertex.color);
        size_t texCoord_hash = hash<glm::vec2>()(vertex.texCoord);

        return ((pos_hash ^ (color_hash << 1)) >> 1) ^ (texCoord_hash << 1);
    }
};
} // namespace std

/* Vertices w/ counter-clockwise winding order
   3                 2
     *-----<<<-----*
     |          __/|
     v       __/   |
     v    >>^      ^
     | __/         ^
     |/            |
     *----->>>-----*
   0                 1
*/
// clang-format off
const vector<Vertex> doublePlaneVertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const vector<uint16_t> doublePlaneIndices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
}; // Make this uint32_t once we get too many vertices
// clang-format on

struct UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
    vec3 _; // Alignment padding
};

namespace FilePaths {
constexpr const char *SHADER_VERT = "shaders/compiled/shader.vert.spv";
constexpr const char *SHADER_FRAG = "shaders/compiled/shader.frag.spv";

constexpr const char *FACE_TEXTURE = "assets/textures/texture.jpg";
constexpr const char *VIKING_ROOM_TEXTURE = "assets/textures/viking_room.png";
constexpr const char *VIKING_ROOM_MODEL = "assets/models/viking_room.obj";
constexpr const char *CHALET_TEXTURE = "assets/textures/chalet.jpg";
constexpr const char *CHALET_MODEL = "assets/models/chalet.obj";

// Nicole model source can be found in the credits in README, FBX importing
// is not (yet?) implemented so I had to convert .obj manually, but will not
// a fbx->obj conversion script (for now?).
constexpr const char *NICOLE_MODEL = "assets/models/Nicole.obj";
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
        throw runtime_error("failed to open file!");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    if (fileSize > std::numeric_limits<std::streamsize>::max()) {
        throw runtime_error("File size exceeds the maximum supported size.");
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
constexpr uint32_t DEFAULT_WINDOW_WIDTH = 1920;
constexpr uint32_t DEFAULT_WINDOW_HEIGHT = 1080;

constexpr auto WINDOW_NAME = "Daniel's 3D Engine";

// For example macbooks have integrated graphics cards, so they would be filtered by this, which wouldn't make sense
constexpr bool ALLOW_DEVICE_WITHOUT_INTEGRATED_GPU = true;
constexpr bool ALLOW_DEVICE_WITHOUT_GEOMETRY_SHADER = true;

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

constexpr vec3 CAMERA_EYE(2.0f, 4.0f, 2.0f);
constexpr vec3 CAMERA_CENTER(0.0f, 0.0f, 0.5f);
constexpr vec3 CAMERA_UP(0.0f, 0.0f, 1.0f);

constexpr float CLIPPING_PLANE_NEAR = 0.1f;
constexpr float CLIPPING_PLANE_FAR = 10.0f;

// Define the preferred surface format as a VkSurfaceFormatKHR struct
constexpr VkSurfaceFormatKHR PREFERRED_SURFACE_FORMAT = {
    VK_FORMAT_B8G8R8A8_SRGB,          // format
    VK_COLOR_SPACE_SRGB_NONLINEAR_KHR // colorSpace
};

} // namespace Settings

#endif // CONSTANTS_H
