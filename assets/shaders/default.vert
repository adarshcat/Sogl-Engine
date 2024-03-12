#version 330 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texture_coord;

uniform mat4 MVP;
uniform mat4 M;
uniform vec3 cameraPos;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main(){
    gl_Position = MVP * vec4(vertex_position,1);
    FragPos = (M * vec4(vertex_position,1)).xyz;
    Normal = mat3(transpose(inverse(M))) * vertex_normal;
    TexCoords = vertex_texture_coord;
}