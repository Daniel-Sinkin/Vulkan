cmake_minimum_required(VERSION 3.10)

project(OpenGLProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLFW
find_package(glfw3 3.3 REQUIRED)

# Find GLEW
find_package(GLEW REQUIRED)

# Include directories
include_directories(${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})

# Add the executable
add_executable(OpenGLProject src/main.cpp)

# Link libraries
target_link_libraries(OpenGLProject ${OPENGL_LIBRARIES} GLEW::GLEW glfw)
