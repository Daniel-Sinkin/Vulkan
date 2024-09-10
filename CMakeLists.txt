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

# Define source files
set(SOURCES
    src/main.cpp
    src/engine.cpp
)

include_directories(include)
include_directories(external)

# Add the executable target
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
        -Wall                        # Enable all common warnings
        -Wextra                      # Enable extra warnings
        -Wpedantic                   # Enforce strict ISO compliance
        -Werror                      # Makes Compiler warnings into Errors instead
        -Wshadow                     # Warn about variable shadowing
        -Wdouble-promotion           # Warn if float is implicitly promoted to double
        -Wnon-virtual-dtor           # Warn when a class with virtual functions lacks a virtual destructor
        -Wold-style-cast             # Warn for C-style casts
        -Wcast-align                 # Warn for potential performance issues due to misaligned casts
        -Wconversion                 # Warn for implicit conversions that may change a value
        -Wsign-conversion            # Warn for implicit sign conversions
        -Wfloat-equal                # Warn if floating-point values are compared for equality
        -Wpointer-arith              # Warn for suspicious pointer arithmetic
        -Wformat=2                   # Check format string usage more thoroughly
        -Wunreachable-code           # Warn on code that will never be executed
        -Wstrict-aliasing=2          # Strict aliasing rules (highest level)
        -Wundefined-func-template    # Warn about undefined function template instantiations
        -fno-common                  # Remove the use of common symbols
        -fstrict-aliasing            # Enable strict aliasing
        -Wshorten-64-to-32           # Warn when implicit truncation from 64-bit to 32-bit happens
        -Wnull-dereference           # Shows places where nullptr are dereferenced
        -Wredundant-move             # Warn for redundant moves
        -Wpessimizing-move           # Warn for pessimizing moves
        -Wformat-nonliteral          # Warn about non-literal format strings
        -Wlifetime                   # Warn about lifetime issues (experimental)
        -Wmissing-field-initializers # Warn if not all fields in a struct are initialized
        -Wimplicit-fallthrough       # Warn if fallthrough is not explicit
        -Wnoexcept                   # Checks that NOEXCEPT functions really can't throw exceptions
        -Wc++2a-compat               # C++23 and up compatibility
        -Wunused-result              # Warn if function results are unused
        -Woverloaded-virtual         # Warn if a function in a derived class hides a non-virtual function in a base class
        -Wlogical-op                 # Warn for suspicious logical operations
        -Wduplicated-cond            # Warn about duplicated conditions in if-else chains
        -Wduplicated-branches        # Warn when if-else branches have identical bodies
        -Wuseless-cast               # Warn if a cast does not change the type or value
        -Wunsafe-loop-optimizations  # Warn about unsafe loop optimizations
        -Wvla-larger-than=1024       # Warn when a VLA is larger than 1024 bytes
        -Weffc++                     # Enable Scott Meyers' Effective C++ warnings
        
        # Runtime Sanitizers are VERY slow, this must be removed later on, should be fine for development though
        # Should make those be set only when project-wide DEBUG flag is set
        -fsanitize=address           # Address Sanitizer
            -fsanitize=undefined         # Undefined Behavior Sanitizer
    )
        target_link_options(VulkanEngine PRIVATE
            -fsanitize=address
            -fsanitize=undefined
        )
    endif()
endif()

