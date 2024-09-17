#pragma once

#include "Constants.h"

struct Transform {
    vec3 position;
    glm::quat rotation;
    vec3 scale;

    Transform();

    mat4 getMatrix() const;

    void rotateEuler(const vec3 &eulerAngles);
    void rotateQuat(const glm::quat &deltaRotation);
    void translate(const vec3 &deltaPosition);
    void scaleBy(const vec3 &scaleFactor);

    [[nodiscard]] vec3 getEulerAngles() const;

    void setRotationFromEuler(const glm::vec3 &eulerAngles);
};