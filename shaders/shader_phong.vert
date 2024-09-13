#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 cameraEye;
    float time;  // Use this to shift by sin(time)
};

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragNormal;
layout(location = 4) out vec3 viewDir;

void main() {
    // Define the rotation angle (22.5 degrees in radians)
    float angle = 3.14159 / 8.0;  // 22.5 degrees in radians

    // Rotation matrix around the y-axis
    mat4 rotationMatrix = mat4(
        cos(angle), 0.0, sin(angle), 0.0,
        0.0,       1.0, 0.0,       0.0,
        -sin(angle), 0.0, cos(angle), 0.0,
        0.0,       0.0, 0.0,       1.0
    );

    // Rotate the position around the y-axis
    vec4 rotatedPosition = rotationMatrix * vec4(inPosition, 1.0);

    // Transform the rotated position into world space
    vec4 worldPosition = model * rotatedPosition;
    fragPosition = worldPosition.xyz;
    fragColor = inColor;
    fragTexCoord = inTexCoord;

    // Calculate the normal in world space
    fragNormal = mat3(transpose(inverse(model))) * inNormal;

    // Calculate view direction
    viewDir = normalize(cameraEye - fragPosition);

    // Final position in clip space
    gl_Position = proj * view * worldPosition;
}