#version 330 core

layout(location = 0) in vec3 render_quad;

out vec2 TexCoords;

void main(){
    gl_Position = vec4(render_quad, 1.0);
    TexCoords = vec2(render_quad.xy/2.0 + vec2(0.5));
}