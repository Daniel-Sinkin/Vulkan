class Game {
public:
    // Constructor taking the GLFW window as an argument
    Game(Engine *engine)
        : m_Engine(engine), lastMouseX(0.0), lastMouseY(0.0), initialMouseState(true),
          m_IsMouseLocked(true), keyBitmask(0), shiftPressed(false) { // Initialize shiftPressed to false
        glfwSetInputMode(m_Engine->getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    // Function to process input and handle game logic
    void handleInput() {
        processMouseMovement();
        processKeyboardInput();
    }

private:
    Engine *m_Engine;
    double lastMouseX, lastMouseY; // To track the last known mouse position
    bool initialMouseState;        // To handle the initial mouse movement
    bool m_IsMouseLocked;          // Variable to lock/unlock the mouse
    uint64_t keyBitmask;           // 64-bit bitmask for key states
    bool shiftPressed;             // Flag to track if Shift key is pressed

    void processKeyboardInput() {
        // Close window if Escape is pressed
        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(m_Engine->getWindow(), true);
        }

        // Handle Tab key to toggle mouse lock/unlock
        if (handleKeyPressReleaseWithBitmask(keyBitmask, KeyBitmask::TAB, GLFW_KEY_TAB, m_Engine->getWindow())) {
            m_IsMouseLocked = !m_IsMouseLocked;

            if (m_IsMouseLocked) {
                // Lock the mouse and hide the cursor
                glfwSetInputMode(m_Engine->getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

                // Reset the mouse to the center of the window
                int windowWidth, windowHeight;
                glfwGetWindowSize(m_Engine->getWindow(), &windowWidth, &windowHeight);
                double centerX = windowWidth / 2.0;
                double centerY = windowHeight / 2.0;

                lastMouseX = centerX;
                lastMouseY = centerY;
                glfwSetCursorPos(m_Engine->getWindow(), centerX, centerY);

                initialMouseState = true; // Reset the initial state to handle the first movement correctly
            } else {
                // Unlock the mouse and show the cursor
                glfwSetInputMode(m_Engine->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }

        if (handleKeyPressReleaseWithBitmask(keyBitmask, KeyBitmask::F12, GLFW_KEY_F12, m_Engine->getWindow())) {
            m_Engine->takeScreenshot();
            std::cout << "Taking screenshot\n";
        }

        // Set or unset the shiftPressed flag
        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(m_Engine->getWindow(), GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
            shiftPressed = true; // Shift is held
        } else {
            shiftPressed = false; // Shift is not held
        }

        // TODO: Add this to Settings
        float speed = 1.0;

        if (shiftPressed) speed *= 2;

        // Example movement keys (WASD)
        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
            m_Engine->moveCameraForward(speed / 60.0);
            std::cout << "Move Forward\n";
        }
        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            m_Engine->moveCameraRight(-speed / 60.0);
            std::cout << "Move Left\n";
        }
        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            m_Engine->moveCameraForward(-speed / 60.0);
            std::cout << "Move Backward\n";
        }
        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
            m_Engine->moveCameraRight(speed / 60.0);
            std::cout << "Move Right\n";
        }
    }

    void processMouseMovement() {
        if (!m_IsMouseLocked) {
            return; // Do not process mouse movement if the mouse is not locked
        }

        // Get the window size
        int windowWidth, windowHeight;
        glfwGetWindowSize(m_Engine->getWindow(), &windowWidth, &windowHeight);

        // Define the center of the window
        double centerX = windowWidth / 2.0;
        double centerY = windowHeight / 2.0;

        // Get current mouse position in window space
        double mouseX, mouseY;
        glfwGetCursorPos(m_Engine->getWindow(), &mouseX, &mouseY);

        // Handle the first mouse event (to avoid a large initial jump)
        if (initialMouseState) {
            lastMouseX = centerX;
            lastMouseY = centerY;
            glfwSetCursorPos(m_Engine->getWindow(), centerX, centerY); // Center the cursor
            initialMouseState = false;
        }

        // Calculate the mouse offset (delta)
        double offsetX = mouseX - lastMouseX;
        double offsetY = lastMouseY - mouseY; // Inverted Y for Vulkan (y-axis goes down in window space)

        // Store the current mouse position for the next frame (centerX and centerY)
        lastMouseX = centerX;
        lastMouseY = centerY;

        // Sensitivity for the camera movement (adjust to your liking)
        float sensitivity = 0.15f;

        // Aspect ratio to normalize pitch movement
        float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

        // Normalize the X and Y sensitivity by window dimensions
        float normalizedYawSensitivity = sensitivity;
        float normalizedPitchSensitivity = sensitivity / aspectRatio;

        // Invert the yaw direction (horizontal movement)
        float yawOffset = static_cast<float>(-offsetX) * normalizedYawSensitivity;
        float pitchOffset = static_cast<float>(offsetY) * normalizedPitchSensitivity;

        // Call the lookAround function to update the camera based on mouse movement
        m_Engine->lookAround(yawOffset, pitchOffset);

        // Reset the mouse cursor to the center of the window
        glfwSetCursorPos(m_Engine->getWindow(), centerX, centerY);
    }
};