#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;  // Normal from vertex shader
layout(location = 3) in float fragTime;

layout(location = 0) out vec4 outColor;

// Lighting parameters
const vec3 lightDirection = normalize(vec3(0.0, -1.0, -1.0)); // Direction of the light
const vec3 lightColor = vec3(1.0, 1.0, 1.0); // White light
const vec3 ambientLight = vec3(0.1, 0.1, 0.1); // Ambient light factor

void main() {
    // Normalize the fragment normal
    vec3 normal = normalize(fragNormal);

    // Compute diffuse lighting factor (dot product of light direction and normal)
    float diffuseFactor = max(dot(normal, lightDirection), 0.0);

    // Combine ambient and diffuse lighting
    vec3 lighting = ambientLight + diffuseFactor * lightColor;

    // Sample the texture and apply lighting
    vec4 texColor = texture(texSampler, fragTexCoord);
    outColor = vec4(texColor.rgb * lighting, texColor.a);
}