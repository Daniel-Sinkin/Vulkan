
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

void ModelNT::translate(const glm::vec3 &deltaPosition) { m_CurrentTransform.translate(deltaPosition); }
void ModelNT::rotate(const glm::vec3 &deltaRotation) { m_CurrentTransform.rotateEuler(deltaRotation); }
void ModelNT::scaleBy(const glm::vec3 &scaleFactor) { m_CurrentTransform.scaleBy(scaleFactor); }
void ModelNT::resetTransform() { m_CurrentTransform = m_InitialTransform; }

const MeshNT *ModelNT::getMesh() const { return m_Mesh.get(); }