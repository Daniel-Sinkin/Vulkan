
#include "Constants.h"

#include "engine/engine.h"
#include "engine/mesh.h"
#include "engine/model.h"
#include "game.h"

using namespace std;

ModelNT::ModelNT(Engine *engine, const char *meshFilepath, uint32_t modelID)
    : m_Engine(engine),
      m_Mesh(nullptr),
      m_MeshFilepath(meshFilepath),
      m_InitialTransform(),
      m_CurrentTransform() {
    m_Mesh = std::make_unique<MeshNT>(engine, meshFilepath);
    m_ModelID = modelID;
    validate();
}

void ModelNT::validate() {
    cout << "Validating mesh.\n";
    if (!m_Mesh) throw runtime_error("Mesh of Model not set!");
    m_Mesh->validate();
    cout << "Mesh is valid.\n";
}

DEF ModelNT::translate(const glm::vec3 &deltaPosition) -> void { m_CurrentTransform.translate(deltaPosition); }
DEF ModelNT::rotate(const glm::vec3 &deltaRotation) -> void { m_CurrentTransform.rotateEuler(deltaRotation); }
DEF ModelNT::scaleBy(const glm::vec3 &scaleFactor) -> void { m_CurrentTransform.scaleBy(scaleFactor); }
DEF ModelNT::resetTransform() -> void { m_CurrentTransform = m_InitialTransform; }

DEF ModelNT::getMesh() const -> MeshNT * { return m_Mesh.get(); }

DEF ModelNT::enqueueIntoCommandBuffer(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet) -> void {
    std::array<VkBuffer, 1> vertexBuffers = {this->getMesh()->getVertexBuffer()};
    std::array<VkDeviceSize, 1> offsets = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers.data(), offsets.data());

    vkCmdBindIndexBuffer(commandBuffer, this->getMesh()->getVertexIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_Engine->getPipelineLayout(),
        0,
        1,
        &descriptorSet,
        0,
        nullptr);

    vkCmdDrawIndexed(
        commandBuffer,
        static_cast<uint32_t>(this->getMesh()->getVertexIndices().size()),
        1,
        0,
        0,
        0);
}

DEF ModelNT::getUBO() -> UniformBufferObject {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    glm::mat4 view = glm::lookAt(m_Engine->m_CameraEye, m_Engine->m_CameraCenter, m_Engine->m_CameraUp);
    glm::mat4 proj = glm::perspective(
        PI_QUARTER,
        static_cast<float>(m_Engine->getSwapchainExtent().width) / static_cast<float>(m_Engine->getSwapchainExtent().height),
        Settings::CLIPPING_PLANE_NEAR, Settings::CLIPPING_PLANE_FAR);

    proj[1][1] *= -1;

    glm::mat4 modelMatrix = this->getMatrix();

    UniformBufferObject ubo{
        .model = modelMatrix,
        .view = view,
        .proj = proj,
        .cameraEye = m_Engine->m_CameraEye,
        .time = delta_time,
        .cameraCenter = m_Engine->m_CameraCenter,
        .cameraUp = m_Engine->m_CameraUp,
        .stage = m_Engine->getStage()};
    return ubo;
}

DEF ModelNT::getMatrix() const -> mat4 {
    return m_CurrentTransform.getMatrix();
}