#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 viewDir;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 cameraEye;
    float time;
};
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    // Transform the normal from world space to view space
    vec3 viewNormal = normalize(mat3(view) * fragNormal);

    // Map the view space normal components (-1 to 1 range) to color components (0 to 1 range)
    vec3 normalColor = (viewNormal * 0.5) + 0.5;

    // Sample the texture color (optional, remove if not needed)
    vec4 sampledColor = texture(texSampler, fragTexCoord);

    // Combine the normal color with the texture's alpha
    outColor = vec4(normalColor, sampledColor.a);
}