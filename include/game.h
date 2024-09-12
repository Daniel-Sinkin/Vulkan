#include "Constants.h"
#include "engine.h"

class Game {
public:
    // Constructor taking the GLFW window as an argument
    Game(Engine *engine)
        : m_Engine(engine), lastMouseX(0.0), lastMouseY(0.0), initialMouseState(true), m_F12HasBeenPressedBefore(false) {}

    // Function to process input and handle game logic
    void handleInput() {
        processMouseMovement();
        processKeyboardInput();
    }

private:
    Engine *m_Engine;
    double lastMouseX, lastMouseY; // To track the last known mouse position
    bool initialMouseState;        // To handle the initial mouse movement

    bool m_F12HasBeenPressedBefore;

    // Process keyboard input
    void processKeyboardInput() {
        // Close window if Escape is pressed
        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(m_Engine->getWindow(), true);
        }

        // Example movement keys (WASD)
        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
            m_Engine->moveCameraForward(1.0 / 60.0);
            std::cout << "Move Forward\n";
        }
        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            m_Engine->moveCameraRight(-1 / 60.0);
            std::cout << "Move Left\n";
        }
        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            m_Engine->moveCameraForward(-1.0 / 60.0);
            std::cout << "Move Backward\n";
        }
        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
            m_Engine->moveCameraRight(1 / 60.0);
            std::cout << "Move Right\n";
        }

        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_F12) == GLFW_PRESS) {
            m_F12HasBeenPressedBefore = true;
        }
        // Take screenshot when F12 is released
        if ((glfwGetKey(m_Engine->getWindow(), GLFW_KEY_F12) == GLFW_RELEASE) & (m_F12HasBeenPressedBefore)) {
            m_Engine->takeScreenshot();
            std::cout << "Taking screenshot\n";

            m_F12HasBeenPressedBefore = false;
        }
    }

    // Print the current mouse position in Vulkan NDC space (-1 to 1)
    void printMousePosition() {
        // Get mouse position in window space
        double mouseX, mouseY;
        glfwGetCursorPos(m_Engine->getWindow(), &mouseX, &mouseY);

        // Get the window size
        int windowWidth, windowHeight;
        glfwGetWindowSize(m_Engine->getWindow(), &windowWidth, &windowHeight);

        // Convert mouse coordinates to Vulkan NDC (-1 to 1 range)
        double vulkanX = (mouseX / windowWidth) * 2.0 - 1.0;  // Normalize to [-1, 1] for x-axis
        double vulkanY = 1.0 - (mouseY / windowHeight) * 2.0; // Normalize to [1, -1] for y-axis

        // Print the mouse position in Vulkan NDC space
        std::cout << "Mouse Position (Vulkan NDC): (" << vulkanX << ", " << vulkanY << ")\n";
    }

    void processMouseMovement() {
        // Get current mouse position in window space
        double mouseX, mouseY;
        glfwGetCursorPos(m_Engine->getWindow(), &mouseX, &mouseY);

        // Handle the first mouse event (to avoid a large initial jump)
        if (initialMouseState) {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            initialMouseState = false;
        }

        // Calculate the mouse offset (delta)
        double offsetX = mouseX - lastMouseX;
        double offsetY = lastMouseY - mouseY; // Inverted Y for Vulkan (y-axis goes down in window space)

        // Store the current mouse position for the next frame
        lastMouseX = mouseX;
        lastMouseY = mouseY;

        // Get the window size
        int windowWidth, windowHeight;
        glfwGetWindowSize(m_Engine->getWindow(), &windowWidth, &windowHeight);

        // Sensitivity for the camera movement (adjust to your liking)
        float sensitivity = 0.15f;

        float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

        // Normalize the X and Y sensitivity by window dimensions
        float normalizedYawSensitivity = sensitivity;
        float normalizedPitchSensitivity = sensitivity / aspectRatio;

        // Invert the yaw direction (horizontal movement)
        float yawOffset = static_cast<float>(-offsetX) * normalizedYawSensitivity;
        float pitchOffset = static_cast<float>(offsetY) * normalizedPitchSensitivity;

        // Call the lookAround function to update the camera based on mouse movement
        m_Engine->lookAround(yawOffset, pitchOffset);
    }
};