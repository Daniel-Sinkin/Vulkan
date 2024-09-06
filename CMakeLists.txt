cmake_minimum_required(VERSION 3.10)

project(VulkanEngine)

# Set C++20 as the standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VULKAN_SDK environment variable is not set")
endif()

set(VULKAN_SDK "$ENV{VULKAN_SDK}")
set(VULKAN_INCLUDE_DIR "${VULKAN_SDK}/include")
set(VULKAN_LIBRARY_DIR "${VULKAN_SDK}/lib")
include_directories(${VULKAN_INCLUDE_DIR})

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

set(SOURCES
    src/main.cpp
)

include_directories(include)

add_executable(VulkanEngine ${SOURCES})

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
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Add Clang-specific warning flags
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
        -Wredundant-move
        -Wpessimizing-move
        -Wformat-nonliteral
        -Wlifetime                   # This is experimental, might remove this later
        -Wmissing-field-initializers
        -Wimplicit-fallthrough
        -Wnoexcept                   # Checks that NOEXCEPT functions really can't throw exceptions (which is very slow)
        -Wc++2a-compat               # C++23 and up compatibility
        
        # Runtime Sanitizers are VERY slow, this must be removed later one, should be fine for development though
        # Should make those be set only when project-wide DEBUG flag is set
        -fsanitize=address           # Adress Sanitizer
        -fsanitize=undefined         # UB sanitizer
    )
endif()