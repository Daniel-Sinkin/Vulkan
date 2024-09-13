#version 450

layout(location = 0) in vec2 fragTexCoord;  // Incoming UV from the vertex shader
layout(location = 0) out vec4 outColor;     // Final color

layout(binding = 0) uniform sampler2D fontAtlas;  // Font texture atlas

void main() {
    // Hardcoded values
    int textLength = 10;                     // Length of the text string
    ivec2 charSize = ivec2(32, 32);          // Size of each character in the atlas (in texels)
    int charsPerRow = 16;                    // Number of characters per row in the atlas

    // Map the fragment coordinate to a normalized (0.0 to 1.0) range per character in the quad
    vec2 localPos = fragTexCoord * vec2(float(textLength), 1.0);  // Scale horizontally by text length

    // Determine which character this fragment belongs to
    int charIndex = int(floor(localPos.x));  // Which character in the string this fragment represents

    // Get the UV offset for the character in the texture atlas
    int row = (charIndex / charsPerRow);
    int col = (charIndex % charsPerRow);

    // Calculate the UV offset in the atlas for this character
    vec2 charUVOffset = vec2(col, row) * vec2(charSize) / textureSize(fontAtlas, 0);

    // Calculate the UV coordinates within the character's box in the atlas
    vec2 charLocalUV = fract(localPos);

    // Final UV coordinate in the texture atlas
    vec2 finalUV = charUVOffset + charLocalUV * vec2(charSize) / textureSize(fontAtlas, 0);

    // Sample the texture atlas to get the color for this character
    float alpha = texture(fontAtlas, finalUV).r;  // Assume the font atlas is grayscale
    outColor = vec4(1.0, 1.0, 1.0, alpha);  // Output white text with the sampled alpha
}