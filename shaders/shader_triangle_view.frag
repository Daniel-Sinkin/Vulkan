#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
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

// Function to generate a pseudo-random color based on the primitive ID
vec3 randomColor(int id) {
    float r = fract(sin(float(id) * 12.9898) * 43758.5453);
    float g = fract(sin(float(id) * 78.233) * 43758.5453);
    float b = fract(sin(float(id) * 39.425) * 43758.5453);
    return vec3(r, g, b);
}

void main() {
    // Get a random color based on the triangle primitive ID
    vec3 color = randomColor(gl_PrimitiveID);

    // Output the final color
    outColor = vec4(color, 1.0);
}