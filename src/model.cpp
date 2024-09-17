
#include "Constants.h"

#include "engine/engine.h"
#include "engine/mesh.h"
#include "engine/model.h"

using namespace std;

ModelNT::ModelNT(Engine *engine, const char *meshFilepath)
    : m_Engine(engine),
      m_Mesh(nullptr),
      m_MeshFilepath(meshFilepath),
      m_InitialTransform(),
      m_CurrentTransform() {
    m_Mesh = std::make_unique<MeshNT>(engine, meshFilepath);
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

void ModelNT::enqueueIntoCommandBuffer(VkCommandBuffer commandBuffer) {
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
        &m_Engine->getDescriptorSets()[m_Engine->getCurrentFrameIdx()],
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