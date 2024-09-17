#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 viewDir;
layout(location = 4) flat in int triangleID;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 cameraEye;
    float time;           // Align to 16 bytes
    vec3 cameraCenter;
    float padding1;       // Alignment padding
    vec3 cameraUp;
    float padding2;       // Alignment padding
    int stage;            // Added the stage variable
};
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

// Stage 0: Discard fragment (render nothing)
void renderStage0() {
    discard;
}

// Stage 1: Output red color
void renderStage1() {
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
}

// Stage 2: Assign random color per triangle based on triangleID
void renderStage2() {
    float r = fract(sin(float(triangleID) * 12.9898) * 43758.5453);
    float g = fract(sin(float(triangleID) * 78.233) * 43758.5453);
    float b = fract(sin(float(triangleID) * 93.9898) * 43758.5453);
    outColor = vec4(r, g, b, 1.0);
}

// Stage 3: Visualize normal vectors
void renderStage3() {
    vec3 normalColor = normalize(fragNormal) * 0.5 + 0.5;
    outColor = vec4(normalColor, 1.0);
}

// Ambient lighting (shared by Stage 4 and above)
vec3 computeAmbientLight() {
    vec3 ambientLightColor = vec3(0.0, 0.0, 0.25);
    float ambientStrength = 0.0001;
    return ambientStrength * ambientLightColor;
}

// Stage 4: Texture without lighting
void renderStage4() {
    vec4 sampledColor = texture(texSampler, fragTexCoord);
    outColor = sampledColor;
}

// Stage 5: Lighting without light movement
void renderStage5(vec3 lightPosition) {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(lightPosition - fragPosition);
    vec3 ambient = computeAmbientLight();
    
    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.5);  // Light color

    vec4 sampledColor = texture(texSampler, fragTexCoord);
    vec3 finalColor = (ambient + diffuse) * sampledColor.rgb;

    // Gamma correction
    float gamma = 2.2;
    vec3 gammaCorrectedColor = pow(finalColor, vec3(1.0 / gamma));

    outColor = vec4(gammaCorrectedColor, sampledColor.a);
}

// Stage 6 & 7: Full lighting with specular and optionally moving light
void renderStage6And7(vec3 lightPosition) {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(lightPosition - fragPosition);
    vec3 viewDirection = normalize(viewDir);
    
    vec3 ambient = computeAmbientLight();
    
    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.5);  // Light color

    // Specular lighting
    float specularStrength = 0.5;
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(0.5);  // Light color

    // Sample texture color
    vec4 sampledColor = texture(texSampler, fragTexCoord);

    // Final color
    vec3 finalColor = (ambient + diffuse + specular) * sampledColor.rgb;

    // Gamma correction
    float gamma = 2.2;
    vec3 gammaCorrectedColor = pow(finalColor, vec3(1.0 / gamma));

    outColor = vec4(gammaCorrectedColor, sampledColor.a);
}

// Main entry point
void main() {
    if (stage == 0) {
        renderStage0();
        return;
    } else if (stage == 1) {
        renderStage1();
        return;
    } else if (stage == 2) {
        renderStage2();
        return;
    } else if (stage == 3) {
        renderStage3();
        return;
    } else if (stage == 4) {
        renderStage4();
        return;
    } else if (stage == 5) {
        renderStage5(vec3(15.0, 0.0, 0.0));  // Static light position
        return;
    } else if (stage == 6 || stage == 7) {
        vec3 lightPosition = (stage == 6) ? vec3(15.0, 0.0, 0.0) : vec3(15.0 * sin(time), 0.0, 0.0);
        renderStage6And7(lightPosition);
        return;
    }
}