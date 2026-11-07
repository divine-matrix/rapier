#version 330 core
in vec2 vUV;
uniform sampler2D tex;
out vec4 fragColor;

void main() {
    fragColor = texture(tex, vUV);  // look up pixel in the GPU texture
} 
