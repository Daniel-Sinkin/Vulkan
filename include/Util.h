#ifndef UTIL_H
#define UTIL_H

#include "Constants.h"

inline void setKeyBit(uint64_t &bitmask, int bitPosition) {
    if (bitPosition < 0 || bitPosition >= 64) throw std::runtime_error("Trying to setKeyBit with invalid bitPosition value: " + std::to_string(bitPosition));
    bitmask |= (1ULL << bitPosition);
}

inline void clearKeyBit(uint64_t &bitmask, int bitPosition) {
    if (bitPosition < 0 || bitPosition >= 64) throw std::runtime_error("Trying to clearKeyBit with invalid bitPosition value: " + std::to_string(bitPosition));
    bitmask &= ~(1ULL << bitPosition);
}

inline bool isKeyBitSet(uint64_t bitmask, int bitPosition) {
    if (bitPosition < 0 || bitPosition >= 64) throw std::runtime_error("Trying to check isKeyBitSet with invalid bitPosition value: " + std::to_string(bitPosition));
    return (bitmask & (1ULL << bitPosition)) != 0;
}

inline bool handleKeyPressReleaseWithBitmask(uint64_t &bitmask, int bitPosition, int key, GLFWwindow *window) {
    if (glfwGetKey(window, key) == GLFW_PRESS) {
        setKeyBit(bitmask, bitPosition);
    }

    if (glfwGetKey(window, key) == GLFW_RELEASE && isKeyBitSet(bitmask, bitPosition)) {
        clearKeyBit(bitmask, bitPosition);
        return true;
    }

    return false;
}

#define VULKAN_SETUP(func)                                                                       \
    {                                                                                            \
        fprintf(stdout, " \033[32m(%zu.) initVulkan Step:\033[0m ", ++initVulkanIteration);      \
        fprintf(stdout, "Trying to initialize %s\n", #func);                                     \
        auto start = std::chrono::high_resolution_clock::now();                                  \
        func();                                                                                  \
        auto end = std::chrono::high_resolution_clock::now();                                    \
        std::chrono::duration<double, std::milli> elapsed = end - start;                         \
        fprintf(stdout, "Successfully initialized %s (Took %.2f ms)\n", #func, elapsed.count()); \
    }

#define TIMED_EXECUTION(func)                                            \
    {                                                                    \
        auto start = std::chrono::high_resolution_clock::now();          \
        func();                                                          \
        auto end = std::chrono::high_resolution_clock::now();            \
        std::chrono::duration<double, std::milli> elapsed = end - start; \
        fprintf(stdout, "%s took %.2f ms\n", #func, elapsed.count());    \
    }

#define PRINT_BOLD_GREEN(text) fprintf(stdout, "\033[1m\033[32m\n%s\n\033[0m", text)

// Overloaded glmPrint for mat4
inline void glmPrint(const glm::mat4 &mat) {
    std::cout << "mat4(" << std::endl;
    for (int i = 0; i < 4; ++i) {
        std::cout << "  " << mat[i][0] << ", " << mat[i][1] << ", " << mat[i][2] << ", " << mat[i][3] << std::endl;
    }
    std::cout << ")" << std::endl;
}

// Overloaded glmPrint for vec2
inline void glmPrint(const glm::vec2 &vec) {
    std::cout << "vec2(" << vec.x << ", " << vec.y << ")" << std::endl;
}

// Overloaded glmPrint for vec3
inline void glmPrint(const glm::vec3 &vec) {
    std::cout << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
}

// Overloaded glmPrint for vec4
inline void glmPrint(const glm::vec4 &vec) {
    std::cout << "vec4(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")" << std::endl;
}

// Overloaded glmPrint for mat3
inline void glmPrint(const glm::mat3 &mat) {
    std::cout << "mat3(" << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << "  " << mat[i][0] << ", " << mat[i][1] << ", " << mat[i][2] << std::endl;
    }
    std::cout << ")" << std::endl;
}

#endif // CONSTANTS_H