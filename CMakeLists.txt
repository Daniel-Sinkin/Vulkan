cmake_minimum_required(VERSION 3.10)

# Set the project name
project(VulkanEngine)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Ensure VULKAN_SDK is set correctly
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VULKAN_SDK environment variable is not set")
endif()

# Set the Vulkan SDK paths
set(VULKAN_SDK "$ENV{VULKAN_SDK}")
set(VULKAN_INCLUDE_DIR "${VULKAN_SDK}/include")
set(VULKAN_LIBRARY_DIR "${VULKAN_SDK}/lib")

# Add the Vulkan SDK include directory
include_directories(${VULKAN_INCLUDE_DIR})

# Find the Vulkan library using CMake's FindVulkan module
find_package(Vulkan REQUIRED)

# Find GLFW package
find_package(glfw3 3.3 REQUIRED)

# Set the source files for the project
set(SOURCES
    src/main.cpp
)

# Create the executable
add_executable(VulkanEngine ${SOURCES})

# Link libraries to the executable
target_link_libraries(VulkanEngine 
    glfw
    Vulkan::Vulkan 
    ${VULKAN_LIBRARY_DIR}/libMoltenVK.dylib
)
