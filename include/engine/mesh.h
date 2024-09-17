#pragma once

#include "Constants.h"
#include "engine/vertex.h"

class Engine; // Forward declaration of Engine class to avoid circular dependency
struct MeshNT {
public:
    MeshNT(Engine *engine, const char *assetFilepath);

    ~MeshNT();

    [[nodiscard]] VkBuffer getVertexBuffer() const;
    [[nodiscard]] VkBuffer getVertexIndexBuffer() const;
    [[nodiscard]] VkDeviceMemory getVertexBufferMemory() const;
    [[nodiscard]] VkDeviceMemory getVertexIndexBufferMemory() const;

    [[nodiscard]] std::vector<VertexNT> getVertices() const;
    [[nodiscard]] std::vector<uint32_t> getVertexIndices() const;

    void loadModel();
    void createVertexBuffer();
    void createIndexBuffer();

    void validate() {
        if (getVertexBuffer() == VK_NULL_HANDLE) throw runtime_error("VertexBuffer is None!");
        if (getVertexBufferMemory() == VK_NULL_HANDLE) throw runtime_error("VertexBufferMemory is None!");
        if (getVertexIndexBuffer() == VK_NULL_HANDLE) throw runtime_error("VertexIndexBuffer is None!");
        if (getVertexIndexBufferMemory() == VK_NULL_HANDLE) throw runtime_error("VertexIndexBufferMemory is None!");
        if (getVertices().size() == 0) throw runtime_error("Vertices are emtpy!");
        if (getVertexIndices().size() == 0) throw runtime_error("VertexIndices are empty!");
    }

private:
    Engine *m_Engine;
    VkDevice m_Device;

    const char *m_Filepath;

    // GPU Memory
    VkBuffer m_VertexBuffer;
    VkDeviceMemory m_VertexBufferMemory;
    VkBuffer m_IndexBuffer;
    VkDeviceMemory m_IndexBufferMemory;

    // CPU Memory
    std::vector<VertexNT> m_Vertices;
    std::vector<uint32_t> m_VertexIndices;
};