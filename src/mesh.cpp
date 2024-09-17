#include "Constants.h"

#include "engine/engine.h"
#include "engine/mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

MeshNT::MeshNT(Engine *engine, const char *assetFilepath) : m_Engine(engine), m_Filepath(assetFilepath) {
    m_Device = m_Engine->getDevice();
    if (m_Device == VK_NULL_HANDLE) {
        throw std::runtime_error("Initializing mesh before engine device got initialized!");
    }
    loadModel();
    createVertexBuffer();
    createIndexBuffer();

    validate();
}

MeshNT::~MeshNT() {
    std::cout << "Cleaning up Mesh.\n";
    vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);
    vkFreeMemory(m_Device, m_IndexBufferMemory, nullptr);
    vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
    vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);
    std::cout << "Finished cleaning up Mesh.\n";
}

// clang-format off
VkBuffer              MeshNT::getVertexBuffer()            const { return m_VertexBuffer;       }
VkBuffer              MeshNT::getVertexIndexBuffer()       const { return m_IndexBuffer;        }
VkDeviceMemory        MeshNT::getVertexBufferMemory()      const { return m_VertexBufferMemory; }
VkDeviceMemory        MeshNT::getVertexIndexBufferMemory() const { return m_IndexBufferMemory;  }
std::vector<VertexNT> MeshNT::getVertices()                const { return m_Vertices;           }
std::vector<uint32_t> MeshNT::getVertexIndices()           const { return m_VertexIndices;      }
// clang-format on 

void MeshNT::loadModel() {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, m_Filepath)) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<VertexNT, uint32_t> uniqueVertices{};

    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            VertexNT vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]};

            if (index.normal_index >= 0) {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]};
            } else {
                vertex.normal = {0.0f, 0.0f, 0.0f};
            }

            if (index.texcoord_index >= 0) {
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
            } else {
                vertex.texCoord = {0.0f, 0.0f};
            }

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
                m_Vertices.push_back(vertex);
            }

            m_VertexIndices.push_back(uniqueVertices[vertex]);
        }
    }

    std::cout << "Number of vertices: " << attrib.vertices.size() / 3 << "\n";
    std::cout << "Number of unique vertices: " << uniqueVertices.size() << "\n";
    std::cout << "Number of indices: " << m_VertexIndices.size() << "\n";
    std::cout << "Number of shapes: " << shapes.size() << "\n";
    std::cout << "Number of materials: " << materials.size() << "\n";
}

void MeshNT::createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
    m_Engine->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory);

    void *data = nullptr;
    vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, m_Vertices.data(), static_cast<uint32_t>(bufferSize));
    vkUnmapMemory(m_Device, stagingBufferMemory);

    m_Engine->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_VertexBuffer,
        m_VertexBufferMemory);

    m_Engine->copyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

    vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
    vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
}

void MeshNT::createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(m_VertexIndices[0]) * m_VertexIndices.size();

    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
    m_Engine->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory);

    void *data = nullptr;
    vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, m_VertexIndices.data(), static_cast<uint32_t>(bufferSize));
    vkUnmapMemory(m_Device, stagingBufferMemory);

    m_Engine->createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_IndexBuffer,
        m_IndexBufferMemory);

    m_Engine->copyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

    vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
    vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
}