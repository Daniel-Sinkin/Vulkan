#include "Constants.h"

#include "transform.h"

Transform::Transform()
    : position(0.0f, 0.0f, 0.0f),
      rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
      scale(1.0f, 1.0f, 1.0f) {}

glm::mat4 Transform::getMatrix() const {
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

    return translationMatrix * rotationMatrix * scaleMatrix;
}

void Transform::rotateEuler(const vec3 &eulerAngles) {
    rotateQuat(glm::quat(eulerAngles));
}

void Transform::rotateQuat(const glm::quat &deltaRotation) {
    rotation = deltaRotation * rotation;
}

void Transform::translate(const vec3 &deltaPosition) {
    position += deltaPosition;
}

void Transform::scaleBy(const vec3 &scaleFactor) {
    scale *= scaleFactor;
}

vec3 Transform::getEulerAngles() const {
    return glm::eulerAngles(rotation);
}

void Transform::setRotationFromEuler(const glm::vec3 &eulerAngles) {
    rotation = glm::quat(eulerAngles);
}