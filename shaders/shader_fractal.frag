#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float fragTime; // Receive time from vertex shader

layout(location = 0) out vec4 outColor;

// A function to create a simple fractal (Julia set) based on the texture coordinates
vec3 fractalPattern(vec2 coord) {
    vec2 z = coord * 2.0 - 1.0;  // Normalize coord to the range [-1, 1]
    vec2 c = vec2(-0.7, 0.27015); // Julia constant (can be adjusted)

    float iterations = 0.0;
    const float maxIterations = 100.0;

    for (int i = 0; i < int(maxIterations); i++) {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (z.y * z.x + z.x * z.y) + c.y;
        z = vec2(x, y);
        
        if (length(z) > 2.0) break; // Escape condition

        iterations += 1.0;
    }

    // Map the iterations to a color: more iterations => brighter color
    return vec3(iterations / maxIterations, iterations / maxIterations, iterations / maxIterations);
}

void main() {
    vec4 sampledColor = texture(texSampler, fragTexCoord);
    
    // Calculate the length (magnitude) of the RGB components of the color
    float colorLength = length(sampledColor.rgb);
    
    // Time modulation factor
    float timeFactor = sin(fragTime) * 0.5 + 0.5; // Ranges from 0 to 1
    
    if (colorLength < 0.8) {
        // Create fractal pattern for colors with length < 0.2
        vec3 fractalColor = fractalPattern(fragTexCoord);
        
        // Flip the fractal color (invert it)
        fractalColor = vec3(1.0) - fractalColor;
        
        // Check if the fractal color is close to black (intensity below a threshold)
        float fractalIntensity = length(fractalColor);
        float threshold = 0.1; // Define the threshold for blackness

        if (fractalIntensity < threshold) {
            // If the fractal part is black, overwrite with fractal
            vec3 blendedColor = mix(sampledColor.rgb, fractalColor, timeFactor);
            outColor = vec4(blendedColor, sampledColor.a);
        } else {
            outColor = sampledColor;
        }
    }
    else {
        // Keep original color for colors with length between 0.2 and 0.8
        outColor = sampledColor;
    }
}
