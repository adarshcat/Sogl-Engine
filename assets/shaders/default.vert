#version 330 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texture_coord;

struct CameraProperties{
    float near;
    float far;
};

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat4 modelMatrix;
uniform CameraProperties camera;

out vec2 texCoord;
out vec3 fragPosView;
out vec3 normal;

void main(){
    gl_Position = mvpMatrix * vec4(vertex_position,1.0);
    fragPosView = (mvMatrix * vec4(vertex_position,1)).xyz;
    normal = mat3(transpose(inverse(modelMatrix))) * vertex_normal;
    texCoord = vertex_texture_coord;
}