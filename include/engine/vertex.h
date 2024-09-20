#include "Constants.h"

struct VertexP {
    vec3 pos;

    static DEF getBindingDescription() -> VkVertexInputBindingDescription {
        return VkVertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(VertexP),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static DEF getAttributeDescriptions() -> std::vector<VkVertexInputAttributeDescription> {
        VkVertexInputAttributeDescription posAttribute{
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexP, pos)};
        return {posAttribute};
    }

    DEF operator==(const VertexP &other) const -> bool {
        return pos == other.pos;
    }
};

struct VertexN {
    vec3 pos;
    vec3 normal;

    static DEF getBindingDescription() -> VkVertexInputBindingDescription {
        return VkVertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(VertexN),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static DEF getAttributeDescriptions() -> std::vector<VkVertexInputAttributeDescription> {
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

    DEF operator==(const VertexN &other) const -> bool {
        return pos == other.pos && normal == other.normal;
    }
};

struct VertexNT {
    vec3 pos;
    vec3 normal;
    vec2 texCoord;

    static DEF getBindingDescription() -> VkVertexInputBindingDescription {
        return VkVertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(VertexNT),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static DEF getAttributeDescriptions() -> std::vector<VkVertexInputAttributeDescription> {
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

    DEF operator==(const VertexNT &other) const -> bool {
        return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
    }
};

struct VertexC {
    vec3 pos;
    vec3 color;

    static DEF getBindingDescription() -> VkVertexInputBindingDescription {
        return VkVertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(VertexC),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static DEF getAttributeDescriptions() -> std::vector<VkVertexInputAttributeDescription> {
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

    DEF operator==(const VertexC &other) const -> bool {
        return pos == other.pos && color == other.color;
    }
};

struct VertexCN {
    vec3 pos;
    vec3 color;
    vec3 normal;

    static DEF getBindingDescription() -> VkVertexInputBindingDescription {
        return VkVertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(VertexCN),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static DEF getAttributeDescriptions() -> std::vector<VkVertexInputAttributeDescription> {
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

    DEF operator==(const VertexCN &other) const -> bool{
        return pos == other.pos && color == other.color && normal == other.normal;
    }
};

namespace std {
template <>
struct hash<VertexP> {
    DEF operator()(const VertexP &vertex) const noexcept -> size_t {
        return std::hash<vec3>()(vertex.pos);
    }
};

template <>
struct hash<VertexN> {
    DEF operator()(const VertexN &vertex) const noexcept -> size_t {
        const size_t h1 = std::hash<vec3>()(vertex.pos);
        const size_t h2 = std::hash<vec3>()(vertex.normal);
        return h1 ^ (h2 << 1);
    }
};

template <>
struct hash<VertexNT> {
    DEF operator()(const VertexNT &vertex) const noexcept -> size_t {
        const size_t h1 = std::hash<vec3>()(vertex.pos);
        const size_t h2 = std::hash<vec3>()(vertex.normal);
        const size_t h3 = std::hash<vec2>()(vertex.texCoord);
        return h1 ^ ((h2 << 1) ^ (h3 << 2));
    }
};

template <>
struct hash<VertexC> {
    DEF operator()(const VertexC &vertex) const noexcept -> size_t {
        const size_t h1 = std::hash<vec3>()(vertex.pos);
        const size_t h2 = std::hash<vec3>()(vertex.color);
        return h1 ^ (h2 << 1);
    }
};

template <>
struct hash<VertexCN> {
    DEF operator()(const VertexCN &vertex) const noexcept -> size_t {
        const size_t h1 = std::hash<vec3>()(vertex.pos);
        const size_t h2 = std::hash<vec3>()(vertex.color);
        const size_t h3 = std::hash<vec3>()(vertex.normal);
        return h1 ^ ((h2 << 1) ^ (h3 << 2));
    }
};
} // namespace std
