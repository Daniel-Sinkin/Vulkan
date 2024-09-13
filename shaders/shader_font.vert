#version 450

layout(location = 0) in vec2 inPosition;  // Vertex position (in screen space or normalized coordinates)
layout(location = 1) in vec2 inTexCoord;  // Texture coordinates (UV)

layout(location = 0) out vec2 fragTexCoord;  // Pass the texture coordinates to the fragment shader

void main() {
    fragTexCoord = inTexCoord;
    
    // Map the vertex position to the NDC (Normalized Device Coordinates)
    gl_Position = vec4(inPosition, 0.0, 1.0);
}