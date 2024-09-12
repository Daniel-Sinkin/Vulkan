#include "Constants.h"
#include "engine.h"

class Game {
public:
    // Constructor taking the GLFW window as an argument
    Game(Engine *engine)
        : m_Engine(engine), lastMouseX(0.0), lastMouseY(0.0), firstMouse(true) {}

    // Function to process input and handle game logic
    void handleInput() {
        processMouseMovement();
        processKeyboardInput();
    }

private:
    Engine *m_Engine;
    double lastMouseX, lastMouseY; // To track the last known mouse position
    bool firstMouse;               // To handle the initial mouse movement

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
            m_Engine->moveCameraRightFree(-1 / 60.0);
            std::cout << "Move Left\n";
        }
        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            m_Engine->moveCameraForward(-1.0 / 60.0);
            std::cout << "Move Backward\n";
        }
        if (glfwGetKey(m_Engine->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
            m_Engine->moveCameraRightFree(1 / 60.0);
            std::cout << "Move Right\n";
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

    // Process mouse movement and use it to move the camera
    void processMouseMovement() {
        // Get current mouse position in window space
        double mouseX, mouseY;
        glfwGetCursorPos(m_Engine->getWindow(), &mouseX, &mouseY);

        // Handle the first mouse event (to avoid a large initial jump)
        if (firstMouse) {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            firstMouse = false;
        }

        // Calculate the mouse offset (delta)
        double offsetX = mouseX - lastMouseX;
        double offsetY = lastMouseY - mouseY; // Inverted Y for Vulkan (y-axis goes down in window space)

        // Store the current mouse position for the next frame
        lastMouseX = mouseX;
        lastMouseY = mouseY;

        // Sensitivity for the camera movement (adjust to your liking)
        float sensitivity = 0.02f;
        // TODO: Fill this up
        // float normalizedYawSensitivity = sensitivity / static_cast<float>(m_Engine->getWindow());    // Adjust X based on width
        // float normalizedPitchSensitivity = sensitivity / static_cast<float>(m_Engine->getWindow()); // Adjust Y based on height

        float yawOffset = static_cast<float>(-offsetX) * sensitivity;  // Horizontal movement
        float pitchOffset = static_cast<float>(offsetY) * sensitivity; // Vertical movement

        // Call the lookAround function to update camera based on mouse movement
        m_Engine->lookAround(yawOffset, pitchOffset);
    }
};