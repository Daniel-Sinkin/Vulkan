#include "game.h"

Game::Game(Engine *engine)
    : m_Engine(engine),
      m_LastMouseX(0.0),
      m_LastMouseY(0.0),
      m_InitialMouseState(true),
      m_CameraEye(Settings::CAMERA_EYE),
      m_CameraCenter(Settings::CAMERA_CENTER),
      m_CameraUp(Settings::CAMERA_UP),
      m_InMenu(false),
      m_KeyBitmask(0),
      m_ShiftPressed(false) {
    glfwSetInputMode(m_Engine->getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Game::update(float frameTime) {
    checkMenuState();
    if (!m_InMenu) handleInput(frameTime);
}

void Game::handleInput(float frameTime) {
    processKeyboardInput(frameTime);
    processMouseMovement(frameTime);
}

void Game::processKeyboardInput(float frameTime) {
    if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_Engine->getWindow(), true);
    }

    if (handleKeyPressReleaseWithBitmask(m_KeyBitmask, KeyBitmask::F12, GLFW_KEY_F12, m_Engine->getWindow())) {
        m_Engine->takeScreenshot();
        std::cout << "Taking screenshot\n";
    }

    if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(m_Engine->getWindow(), GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        m_ShiftPressed = true;
    } else {
        m_ShiftPressed = false;
    }

    float speed = m_ShiftPressed ? Settings::CAMERA_FLOATING_SPEED_BOOSTED : Settings::CAMERA_FLOATING_SPEED;
    speed *= frameTime;

    if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
        m_Engine->moveCameraForward(speed);
        std::cout << "Move Forward\n";
    }
    if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
        m_Engine->moveCameraRight(-speed);
        std::cout << "Move Left\n";
    }
    if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
        m_Engine->moveCameraForward(-speed);
        std::cout << "Move Backward\n";
    }
    if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
        m_Engine->moveCameraRight(speed);
        std::cout << "Move Right\n";
    }
}

void Game::checkMenuState() {
    if (handleKeyPressReleaseWithBitmask(m_KeyBitmask, KeyBitmask::TAB, GLFW_KEY_TAB, m_Engine->getWindow())) {
        m_InMenu = !m_InMenu;

        if (m_InMenu) {
            glfwSetInputMode(m_Engine->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(m_Engine->getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            int windowWidth = 0;
            int windowHeight = 0;
            glfwGetWindowSize(m_Engine->getWindow(), &windowWidth, &windowHeight);
            double centerX = static_cast<double>(windowWidth) * WINDOW_CENTER_FACTOR;
            double centerY = static_cast<double>(windowHeight) * WINDOW_CENTER_FACTOR;

            m_LastMouseX = centerX;
            m_LastMouseY = centerY;
            glfwSetCursorPos(m_Engine->getWindow(), centerX, centerY);

            m_InitialMouseState = true;
        }
    }
}

void Game::processMouseMovement(float frameTime) {
    int windowWidth = 0;
    int windowHeight = 0;
    glfwGetWindowSize(m_Engine->getWindow(), &windowWidth, &windowHeight);

    double centerX = static_cast<double>(windowWidth) * WINDOW_CENTER_FACTOR;
    double centerY = static_cast<double>(windowHeight) * WINDOW_CENTER_FACTOR;

    double mouseX = 0.0;
    double mouseY = 0.0;
    glfwGetCursorPos(m_Engine->getWindow(), &mouseX, &mouseY);

    if (m_InitialMouseState) {
        m_LastMouseX = centerX;
        m_LastMouseY = centerY;
        glfwSetCursorPos(m_Engine->getWindow(), centerX, centerY);
        m_InitialMouseState = false;
    }

    double offsetX = mouseX - m_LastMouseX;
    double offsetY = m_LastMouseY - mouseY;

    m_LastMouseX = centerX;
    m_LastMouseY = centerY;

    float offsetAmount = Settings::MOUSE_SENSITIVITY * frameTime;

    float yawOffset = static_cast<float>(-offsetX) * offsetAmount;
    float pitchOffset = static_cast<float>(offsetY) * offsetAmount;

    m_Engine->lookAround(yawOffset, pitchOffset);

    glfwSetCursorPos(m_Engine->getWindow(), centerX, centerY);
}