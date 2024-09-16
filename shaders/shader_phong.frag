#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragNormal;
layout(location = 4) in vec3 viewDir;


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
    vec3 normal = normalize(fragNormal);

    float lightHeight = 5;
    float lightRadius = 2.0;

    vec3 lightPosition = vec3(15 * sin(time), 0.0, 0.0);
    vec3 lightColor;
    float modTime = mod(time, 3.0);
    float phase = 3.14569 * (time + 1.0);

    // float lightIntensity = sin(phase) * sin(phase);
    float lightIntensity = 1.0;

    lightColor = vec3(0.5);

    // Ambient light properties
    vec3 ambientLightColor = vec3(0.0, 0.0, 0.25);
    float ambientStrength = 0.0001;

    vec3 lightDir = normalize(lightPosition - fragPosition);
    vec3 viewDirection = normalize(viewDir);

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

    // Apply gamma correction
    float gamma = 2.2;
    vec3 gammaCorrectedColor = pow(finalColor, vec3(1.0 / gamma));

    // Output the final color with texture alpha
    outColor = vec4(gammaCorrectedColor, sampledColor.a);
}