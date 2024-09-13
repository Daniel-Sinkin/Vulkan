#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec3 viewDir;

layout(binding = 1) uniform sampler2D texSampler; // Texture sampler

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 cameraEye;
    float time; // Access time from UniformBufferObject
};

layout(location = 0) out vec4 outColor;

void main() {
    vec3 normal = normalize(fragNormal);

    // Light properties
    float lightHeight = 5; // Constant height for the light
    float lightRadius = 2.0; // Radius of the circular rotation

    // Compute rotating light position based on time
    vec3 lightPosition = vec3(15 * sin(time), 0.0, 0.0);
    vec3 lightColor;
    // Adjusted modulo time for synchronized transitions
    float modTime = mod(time, 3.0);
    float phase = 3.14569 * (time + 1.0);  // Phase shift for the sine wave

    // Calculate light intensity with sine wave, ensuring the intensity is between 0 and 1
    // float lightIntensity = sin(phase) * sin(phase);
    float lightIntensity = 1.0;

    // Change color when light intensity reaches 0 (modulo time divisions)
    if (modTime < 1.0) {
        lightColor = vec3(1.0, 0.0, 0.0);  // Red color
    } else if (modTime < 2.0) {
        lightColor = vec3(0.0, 1.0, 0.0);  // Green color
    } else {
        lightColor = vec3(0.0, 0.0, 1.0);  // Blue color
    }
    lightColor = vec3(1.0, 1.0, 1.0);

    // Ambient light properties
    vec3 ambientLightColor = vec3(0.3, 0.3, 0.3);  // Ambient light color
    float ambientStrength = 0.1;                   // Ambient light strength

    vec3 lightDir = normalize(lightPosition - fragPosition);
    vec3 viewDirection = normalize(viewDir);

    // Ambient lighting
    vec3 ambient = ambientStrength * ambientLightColor;

    // Diffuse lighting (Lambertian reflectance)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting (Phong reflectance)
    float specularStrength = 0.5;
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Sample the texture color using the texture coordinates
    vec4 sampledColor = texture(texSampler, fragTexCoord);

    // Combine the lighting components with the texture color and fragment color
    vec3 finalColor = (ambient + diffuse + specular) * lightIntensity * sampledColor.rgb * fragColor;

    // Output the final color with texture alpha
    outColor = vec4(finalColor, sampledColor.a);
}