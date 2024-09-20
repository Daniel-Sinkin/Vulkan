#pragma once

#include "Constants.h"
#include "engine/mesh.h"

class Engine; // Forward declaration of Engine class to avoid circular dependency
class ModelNT {
public:
    ModelNT(Engine *engine, const char *meshFilepath, uint32_t modelID);
    ModelNT(Engine *engine, const char *meshFilepath, uint32_t modelID, Transform initialTransform);

    ~ModelNT() = default;

    DEF validate() -> void;

    DEF translate(const vec3 &deltaPosition) -> void;
    DEF rotate(const vec3 &deltaRotation) -> void;
    DEF scaleBy(const vec3 &scaleFactor) -> void;

    DEF resetTransform() -> void;

    DEF getMesh() const -> MeshNT *;
    [[nodiscard]] DEF getMatrix() const -> mat4 { return m_CurrentTransform.getMatrix(); }

    DEF enqueueIntoCommandBuffer(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet) -> void;
    [[nodiscard]] DEF getUBO() -> UniformBufferObject;

    DEF setRotationAnimationVector(const vec3 rotationAnimationVector) -> void { m_RotationAnimationVector = rotationAnimationVector; }

    DEF update(const float frameTime) -> void { rotate(m_RotationAnimationVector * frameTime); }

    Transform m_CurrentTransform;

private:
    Engine *m_Engine;
    std::unique_ptr<MeshNT> m_Mesh;
    const char *m_MeshFilepath;

    Transform m_InitialTransform;

    uint32_t m_ModelID;

    vec3 m_RotationAnimationVector;
};