#include "game.h"

#include "Util.h"

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
    m_Window = m_Engine->getWindow();
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
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
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_Window, true);
    }

    if (handleKeyPressReleaseWithBitmask(m_KeyBitmask, KeyBitmask::F12, GLFW_KEY_F12, m_Window)) {
        m_Engine->takeScreenshot();
        std::cout << "Taking screenshot\n";
    }

    bool leftShiftPressed = glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    bool rightShiftPressed = glfwGetKey(m_Window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
    m_ShiftPressed = rightShiftPressed || leftShiftPressed;

    float speed = m_ShiftPressed ? Settings::CAMERA_FLOATING_SPEED_BOOSTED : Settings::CAMERA_FLOATING_SPEED;
    speed *= frameTime;

    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS) m_Engine->moveCameraForward(speed);
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS) m_Engine->moveCameraRight(-speed);
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS) m_Engine->moveCameraForward(-speed);
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS) m_Engine->moveCameraRight(speed);

    HANDLE_STAGE_INPUT(0);
    HANDLE_STAGE_INPUT(1);
    HANDLE_STAGE_INPUT(2);
    HANDLE_STAGE_INPUT(3);
    HANDLE_STAGE_INPUT(4);
    HANDLE_STAGE_INPUT(5);
    HANDLE_STAGE_INPUT(6);
    HANDLE_STAGE_INPUT(7);
    HANDLE_STAGE_INPUT(8);
    HANDLE_STAGE_INPUT(9);
}

void Game::checkMenuState() {
    if (handleKeyPressReleaseWithBitmask(m_KeyBitmask, KeyBitmask::TAB, GLFW_KEY_TAB, m_Window)) {
        m_InMenu = !m_InMenu;

        if (m_InMenu) {
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            int windowWidth = 0;
            int windowHeight = 0;
            glfwGetWindowSize(m_Window, &windowWidth, &windowHeight);
            double centerX = static_cast<double>(windowWidth) * WINDOW_CENTER_FACTOR;
            double centerY = static_cast<double>(windowHeight) * WINDOW_CENTER_FACTOR;

            m_LastMouseX = centerX;
            m_LastMouseY = centerY;
            glfwSetCursorPos(m_Window, centerX, centerY);

            m_InitialMouseState = true;
        }
    }
}

void Game::processMouseMovement(float frameTime) {
    int windowWidth = 0;
    int windowHeight = 0;
    glfwGetWindowSize(m_Window, &windowWidth, &windowHeight);

    double centerX = static_cast<double>(windowWidth) * WINDOW_CENTER_FACTOR;
    double centerY = static_cast<double>(windowHeight) * WINDOW_CENTER_FACTOR;

    double mouseX = 0.0;
    double mouseY = 0.0;
    glfwGetCursorPos(m_Window, &mouseX, &mouseY);

    if (m_InitialMouseState) {
        m_LastMouseX = centerX;
        m_LastMouseY = centerY;
        glfwSetCursorPos(m_Window, centerX, centerY);
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

    glfwSetCursorPos(m_Window, centerX, centerY);
}