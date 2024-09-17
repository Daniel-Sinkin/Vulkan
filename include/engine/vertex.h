#include "Constants.h"

struct VertexP {
    glm::vec3 pos;

    static VkVertexInputBindingDescription getBindingDescription() {
        return VkVertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(VertexP),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        VkVertexInputAttributeDescription posAttribute{
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexP, pos)};
        return {posAttribute};
    }

    bool operator==(const VertexP &other) const {
        return pos == other.pos;
    }
};

struct VertexN {
    glm::vec3 pos;
    glm::vec3 normal;

    static VkVertexInputBindingDescription getBindingDescription() {
        return VkVertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(VertexN),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        VkVertexInputAttributeDescription posAttribute{
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexN, pos)};
        VkVertexInputAttributeDescription normalAttribute{
            .binding = 0,
            .location = 1,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexN, normal)};
        return {posAttribute, normalAttribute};
    }

    bool operator==(const VertexN &other) const {
        return pos == other.pos && normal == other.normal;
    }
};

struct VertexNT {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        return VkVertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(VertexNT),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        VkVertexInputAttributeDescription posAttribute{
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexNT, pos)};
        VkVertexInputAttributeDescription normalAttribute{
            .binding = 0,
            .location = 1,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexNT, normal)};
        VkVertexInputAttributeDescription texCoordAttribute{
            .binding = 0,
            .location = 2,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(VertexNT, texCoord)};
        return {posAttribute, normalAttribute, texCoordAttribute};
    }

    bool operator==(const VertexNT &other) const {
        return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
    }
};

struct VertexC {
    glm::vec3 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        return VkVertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(VertexC),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        VkVertexInputAttributeDescription posAttribute{
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexC, pos)};
        VkVertexInputAttributeDescription colorAttribute{
            .binding = 0,
            .location = 1,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexC, color)};
        return {posAttribute, colorAttribute};
    }

    bool operator==(const VertexC &other) const {
        return pos == other.pos && color == other.color;
    }
};

struct VertexCN {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;

    static VkVertexInputBindingDescription getBindingDescription() {
        return VkVertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(VertexCN),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        VkVertexInputAttributeDescription posAttribute{
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexCN, pos)};
        VkVertexInputAttributeDescription colorAttribute{
            .binding = 0,
            .location = 1,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexCN, color)};
        VkVertexInputAttributeDescription normalAttribute{
            .binding = 0,
            .location = 2,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexCN, normal)};
        return {posAttribute, colorAttribute, normalAttribute};
    }

    bool operator==(const VertexCN &other) const {
        return pos == other.pos && color == other.color && normal == other.normal;
    }
};

namespace std {
template <>
struct hash<VertexP> {
    size_t operator()(const VertexP &vertex) const {
        return std::hash<glm::vec3>()(vertex.pos);
    }
};

template <>
struct hash<VertexN> {
    size_t operator()(const VertexN &vertex) const {
        size_t h1 = std::hash<glm::vec3>()(vertex.pos);
        size_t h2 = std::hash<glm::vec3>()(vertex.normal);
        return h1 ^ (h2 << 1);
    }
};

template <>
struct hash<VertexNT> {
    size_t operator()(const VertexNT &vertex) const {
        size_t h1 = std::hash<glm::vec3>()(vertex.pos);
        size_t h2 = std::hash<glm::vec3>()(vertex.normal);
        size_t h3 = std::hash<glm::vec2>()(vertex.texCoord);
        return h1 ^ ((h2 << 1) ^ (h3 << 2));
    }
};

template <>
struct hash<VertexC> {
    size_t operator()(const VertexC &vertex) const {
        size_t h1 = std::hash<glm::vec3>()(vertex.pos);
        size_t h2 = std::hash<glm::vec3>()(vertex.color);
        return h1 ^ (h2 << 1);
    }
};

template <>
struct hash<VertexCN> {
    size_t operator()(const VertexCN &vertex) const {
        size_t h1 = std::hash<glm::vec3>()(vertex.pos);
        size_t h2 = std::hash<glm::vec3>()(vertex.color);
        size_t h3 = std::hash<glm::vec3>()(vertex.normal);
        return h1 ^ ((h2 << 1) ^ (h3 << 2));
    }
};
} // namespace std
