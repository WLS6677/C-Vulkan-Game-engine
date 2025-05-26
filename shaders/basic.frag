#version 450

//input from vertex shader
layout (location = 0) in vec4 fragColor;

// Output to the framebuffer
layout(location = 0) out vec4 outColor;

void main(){
    outColor = fragColor;
}