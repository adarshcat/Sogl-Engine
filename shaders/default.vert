#version 330 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;

uniform mat4 MVP;
uniform mat4 M;
uniform vec3 cameraPos;
uniform vec3 albedo;

out vec3 fragPos;
out vec3 worldNormal;

void main(){
    gl_Position = MVP * vec4(vertex_position,1);
    fragPos = (M * vec4(vertex_position,1)).xyz;
    worldNormal = mat3(transpose(inverse(M))) * vertex_normal;
}