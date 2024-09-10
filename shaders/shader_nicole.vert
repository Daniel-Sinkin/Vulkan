#version 450

// Uniform buffer with the new time field
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;   // Added the time field
    vec3 _;       // Padding to maintain alignment
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out float fragTime;    // Pass time to the fragment shader
layout(location = 3) out float vertYPos;    // Pass the y-coordinate of the vertex

void main() {
    // Transform the vertex position by the model, view, and projection matrices
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);

    // Pass input attributes to the fragment shader
    fragColor = inColor;
    fragTexCoord = inTexCoord;

    // Pass the time and the y-coordinate to the fragment shader
    fragTime = ubo.time;
    vertYPos = inPosition.y;  // Pass the vertex y-position
}