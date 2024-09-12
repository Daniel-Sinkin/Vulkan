#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;   // Time field
    vec3 _;       // Padding to maintain alignment
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal; // New: Normal from vertex

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal; // New: Passing normal to fragment shader
layout(location = 3) out float fragTime; // Pass time to fragment shader

void main() {
    // Compute vertex position in clip space
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);

    // Pass color and texture coordinates
    fragColor = inColor;
    fragTexCoord = inTexCoord;

    // Pass normal, transforming it by the model matrix to get it in world space
    fragNormal = mat3(transpose(inverse(ubo.model))) * inNormal;

    // Pass the time value
    fragTime = ubo.time;
}