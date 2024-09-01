cmake_minimum_required(VERSION 3.10)

project(VulkanEngine)

set(CMAKE_CXX_STANDARD 17)
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
