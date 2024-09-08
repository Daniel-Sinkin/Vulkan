#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

// Define a fixed world-space position for brightness calculations
const vec3 fixedPosition = vec3(0.0, 1.0, 0.0); // Example: origin or any other fixed point

void main() {
    // Compute world-space position by applying the model matrix
    vec4 worldPosition = ubo.model * vec4(inPosition, 0.0, 1.0);

    // Calculate the distance from the fixed world-space position
    float distance = length(worldPosition.xyz - fixedPosition);

    // Calculate the brightness factor using the inverse square law
    // Prevent division by zero by adding a small offset to the distance
    float brightness = 1.0 / (distance * distance + 0.1); // Adding 0.1 to avoid extreme brightness near zero distance

    // Create a base color based on the world position with some purple/green bias
    vec3 baseColor = vec3(0.8 * worldPosition.x + 0.5, 0.5 * worldPosition.y + 0.3, 1.0 * worldPosition.z + 0.6);

    // Apply brightness factor to adjust the color intensity
    fragColor = baseColor * brightness;

    // Clamp final color to avoid oversaturation
    fragColor = clamp(fragColor, 0.0, 1.0);

    // Pass the final transformed position to gl_Position
    gl_Position = ubo.proj * ubo.view * worldPosition;
}