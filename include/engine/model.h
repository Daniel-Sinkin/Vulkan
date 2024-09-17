#pragma once

#include "Constants.h"
#include "engine/mesh.h"

class Engine; // Forward declaration of Engine class to avoid circular dependency
class ModelNT {
public:
    ModelNT(Engine *engine, const char *meshFilepath);

    ~ModelNT() = default;

    void validate();

    void translate(const glm::vec3 &deltaPosition);
    void rotate(const glm::vec3 &deltaRotation);
    void scaleBy(const glm::vec3 &scaleFactor);

    void resetTransform();

    DEF getMesh() const -> MeshNT *;
    [[nodiscard]] DEF getMatrix() const -> mat4;

    void enqueueIntoCommandBuffer(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet);

private:
    Engine *m_Engine;
    std::unique_ptr<MeshNT> m_Mesh;
    const char *m_MeshFilepath;

    Transform m_InitialTransform;
    Transform m_CurrentTransform;
};