#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
};

// Push constant block
layout(push_constant) uniform PushConstants {
    vec3 cameraEye; 
    vec3 cameraCenter;
    vec3 cameraUp;
    float time;
    int stage;
} pc; 

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 viewDir;
layout(location = 4) flat out int triangleID;  // Flat shading for triangle IDs

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
    fragTexCoord = inTexCoord;

    // Calculate the normal in world space
    fragNormal = mat3(transpose(inverse(model))) * inNormal;

    // Calculate view direction
    viewDir = normalize(pc.cameraEye - fragPosition);

    triangleID = gl_VertexIndex;

    // Final position in clip space
    gl_Position = proj * view * worldPosition;
}