#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 cameraEye;
    float time;  // Use this to shift by sin(time)
};

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 viewDir;

void main() {
    // Transform the rotated position into world space
    vec4 worldPosition = model * vec4(inPosition, 1.0);
    fragPosition = worldPosition.xyz;
    fragTexCoord = inTexCoord;

    // Calculate the normal in world space
    fragNormal = mat3(transpose(inverse(model))) * inNormal;

    // Calculate view direction
    viewDir = normalize(cameraEye - fragPosition);

    // Final position in clip space
    gl_Position = proj * view * worldPosition;
}