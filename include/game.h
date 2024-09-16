#pragma once

#include "engine/engine.h"

class Game {
public:
    explicit Game(Engine *engine);
    void update(float frameTime);

private:
    void handleInput(float frameTime);

    void processKeyboardInput(float frameTime);

    void checkMenuState();

    void processMouseMovement(float frameTime);

    Engine *m_Engine;
    GLFWwindow *m_Window;

    double m_LastMouseX, m_LastMouseY;
    bool m_InitialMouseState;
    bool m_InMenu;
    uint64_t m_KeyBitmask;
    bool m_ShiftPressed;

    vec3 m_CameraEye;
    vec3 m_CameraCenter;
    vec3 m_CameraUp;
};