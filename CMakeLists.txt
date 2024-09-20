cmake_minimum_required(VERSION 3.10)

project(VulkanEngine)

# Set the standard and export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Check if the VULKAN_SDK environment variable is set
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VULKAN_SDK environment variable is not set")
endif()

# Set Vulkan SDK paths
set(VULKAN_SDK "$ENV{VULKAN_SDK}")
set(VULKAN_INCLUDE_DIR "${VULKAN_SDK}/include")
set(VULKAN_LIBRARY_DIR "${VULKAN_SDK}/lib")
include_directories(${VULKAN_INCLUDE_DIR})

# Find required packages
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

file(GLOB SOURCES "src/*.cpp")

include_directories(include)
include_directories(external) # Suppresses warnings from external libraries
include_directories(${GLM_INCLUDE_DIRS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_executable(VulkanEngine ${SOURCES})

# Link necessary libraries
target_link_libraries(VulkanEngine
        glfw
        Vulkan::Vulkan
        ${VULKAN_LIBRARY_DIR}/libMoltenVK.dylib
)

# Set the default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Set flags for different build types
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")  # Optimize for release and disable assertions

# Disable interprocedural optimization for debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
endif()

# Add Clang-specific warning flags and sanitizers only for Debug builds
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(VulkanEngine PRIVATE
            # Warning flags here...
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_options(VulkanEngine PRIVATE
                -fsanitize=address
                -fsanitize=undefined
        )
    endif()
endif()

# Custom command to run the Python script with python3
add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/shapes_generated
        COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/generate_shapes.py
        DEPENDS ${CMAKE_SOURCE_DIR}/scripts/generate_shapes.py
        COMMENT "Running generate_shapes.py"
)

# Custom command to run the shell script
add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/shaders_compiled
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/compile_shaders.sh
        DEPENDS ${CMAKE_SOURCE_DIR}/scripts/compile_shaders.sh
        COMMENT "Running compile_shaders.sh"
)

# Custom target that depends on both the Python and shell script outputs
add_custom_target(GenerateShapesAndCompileShaders
        DEPENDS ${CMAKE_SOURCE_DIR}/shapes_generated ${CMAKE_SOURCE_DIR}/shaders_compiled
)

# Ensure the custom target runs before building VulkanEngine
add_dependencies(VulkanEngine GenerateShapesAndCompileShaders)