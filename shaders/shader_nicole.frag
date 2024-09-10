#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float fragTime;  // Time passed from vertex shader
layout(location = 3) in float vertYPos;  // Vertex y-position passed from vertex shader

layout(location = 0) out vec4 outColor;

void main() {
    // Base color for human-like skin (light reddish or beige tone)
    vec3 baseColor = vec3(0.8, 0.6, 0.5);

    // Stronger secondary color (bluish tint) for more contrast
    vec3 secondaryColor = vec3(0.3, 0.4, 0.8);

    // Third color (e.g., a reddish tint for a more dramatic effect)
    vec3 thirdColor = vec3(0.9, 0.3, 0.3);

    // Invert the vertical variation to make the effect move from top to bottom
    float verticalVariation = 2 - vertYPos * 0.2;  // Adjust the influence of vertYPos, inverted for top-down effect

    // Introduce time-based variation with a stronger effect for more visible animation
    float timeFactor = sin(fragTime * 2.5);  // Increase frequency for faster oscillation

    // Combine timeFactor with verticalVariation to blend colors over time
    float combinedFactor = verticalVariation + timeFactor * 0.5;  // Make time variation more visible

    // First blend between baseColor and secondaryColor
    vec3 midColor = mix(baseColor, secondaryColor, combinedFactor);  // Intermediate blend using combinedFactor

    // Second blend between the result (midColor) and thirdColor
    vec3 finalColor = mix(midColor, thirdColor, combinedFactor * 0.5);  // Blend towards thirdColor using combined factor

    // Output the final color
    outColor = vec4(finalColor, 1.0);
}