#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;  // Normal from vertex shader
layout(location = 3) in float fragTime;

layout(location = 0) out vec4 outColor;

// Lighting parameters
const vec3 ambientLight = vec3(0.1, 0.1, 0.1); // Ambient light factor
const vec3 lightDirection1 = normalize(vec3(1.0, 0.0, 0.0)); // Direction for the red light (X-axis)
const vec3 lightDirection2 = normalize(vec3(0.0, 1.0, 0.0)); // Direction for the green light (Y-axis)
const vec3 lightDirection3 = normalize(vec3(0.0, 0.0, 1.0)); // Direction for the blue light (Z-axis)

// Light intensity multipliers (increase the strength of the lights)
const float lightStrength = 2.5; // Multiplier for light intensity
const float sharpnessFactor = 5.0; // Sharpen the diffuse lighting (reduce diffuseness)

void main() {
    // Normalize the fragment normal
    vec3 normal = normalize(fragNormal);

    // Oscillate each light color based on fragTime with different phases for each light
    float oscillationRed = sin(fragTime * 2.0 + 0.0);   // Red light (no phase shift)
    float oscillationGreen = sin(fragTime * 2.0 + 1.0); // Green light (phase shift)
    float oscillationBlue = sin(fragTime * 2.0 + 2.0);  // Blue light (larger phase shift)

    // For each light direction, use the sign of the oscillation to determine which direction is active
    float diffuseFactor1 = (oscillationRed > 0.0) ? max(dot(normal, lightDirection1), 0.0)
                                                  : max(dot(normal, -lightDirection1), 0.0);
    float diffuseFactor2 = (oscillationGreen > 0.0) ? max(dot(normal, lightDirection2), 0.0)
                                                    : max(dot(normal, -lightDirection2), 0.0);
    float diffuseFactor3 = (oscillationBlue > 0.0) ? max(dot(normal, lightDirection3), 0.0)
                                                   : max(dot(normal, -lightDirection3), 0.0);

    // Sharpen the diffuse factors by raising them to a power
    diffuseFactor1 = pow(diffuseFactor1, sharpnessFactor);
    diffuseFactor2 = pow(diffuseFactor2, sharpnessFactor);
    diffuseFactor3 = pow(diffuseFactor3, sharpnessFactor);

    // Adjust oscillation values to oscillate between 0 and 1 for brightness control
    vec3 redLightColor = vec3(1.0, 0.0, 0.0) * abs(oscillationRed) * lightStrength;
    vec3 greenLightColor = vec3(0.0, 1.0, 0.0) * abs(oscillationGreen) * lightStrength;
    vec3 blueLightColor = vec3(0.0, 0.0, 1.0) * abs(oscillationBlue) * lightStrength;

    // Combine ambient and diffuse lighting for each light
    vec3 lightingRed = ambientLight + diffuseFactor1 * redLightColor;
    vec3 lightingGreen = ambientLight + diffuseFactor2 * greenLightColor;
    vec3 lightingBlue = ambientLight + diffuseFactor3 * blueLightColor;

    // Combine the three light contributions
    vec3 totalLighting = lightingRed + lightingGreen + lightingBlue;

    // Sample the texture and apply lighting
    vec4 texColor = texture(texSampler, fragTexCoord);
    outColor = vec4(texColor.rgb * totalLighting, texColor.a);
}
