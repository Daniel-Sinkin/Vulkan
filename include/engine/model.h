#pragma once

#include "Constants.h"
#include "engine/mesh.h"

class Engine; // Forward declaration of Engine class to avoid circular dependency
class ModelNT {
public:
    ModelNT(Engine *engine, const char *meshFilepath, uint32_t modelID);

    ~ModelNT() = default;

    DEF validate() -> void;

    DEF translate(const glm::vec3 &deltaPosition) -> void;
    DEF rotate(const glm::vec3 &deltaRotation) -> void;
    DEF scaleBy(const glm::vec3 &scaleFactor) -> void;

    DEF resetTransform() -> void;

    DEF getMesh() const -> MeshNT *;
    [[nodiscard]] DEF getMatrix() const -> mat4;

    DEF enqueueIntoCommandBuffer(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet) -> void;
    DEF getUBO() -> UniformBufferObject;

private:
    Engine *m_Engine;
    std::unique_ptr<MeshNT> m_Mesh;
    const char *m_MeshFilepath;

    Transform m_InitialTransform;
    Transform m_CurrentTransform;

    uint32_t m_ModelID;
};