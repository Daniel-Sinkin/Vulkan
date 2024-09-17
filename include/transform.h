#pragma once

#include "Constants.h"

struct Transform {
    vec3 position;
    glm::quat rotation;
    vec3 scale;

    Transform();
    Transform(const vec3 &pos, const glm::quat &rot, const vec3 &scl);
    Transform(const vec3 &pos, const vec3 &rotEuler, const vec3 &scl);

    mat4 getMatrix() const;

    void rotateEuler(const vec3 &eulerAngles);
    void rotateQuat(const glm::quat &deltaRotation);
    void translate(const vec3 &deltaPosition);
    void scaleBy(const vec3 &scaleFactor);

    [[nodiscard]] vec3 getEulerAngles() const;

    void setRotationFromEuler(const glm::vec3 &eulerAngles);
};