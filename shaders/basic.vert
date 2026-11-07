#version 330 core
layout(location=0) in vec2 aPos;   // clip-space corner: (-1,-1), (1,-1), (-1,1), (1,1)
layout(location=1) in vec2 aUV;    // texture coord: (0,0), (1,0), (0,1), (1,1)
out vec2 vUV;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);  // already in clip space, no transform
    vUV = aUV;                            // pass through to fragment
}
