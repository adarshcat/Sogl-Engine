#version 330 core

layout (location = 0) out vec3 gNormal;
layout (location = 1) out vec4 gAlbedoSpec;

struct CameraProperties{
    float near;
    float far;
};

in vec2 texCoord;
in vec3 fragPosView;
in vec3 normal;

uniform CameraProperties camera;
uniform vec3 albedo;

void main() {
    gNormal = normalize(normal);
    gAlbedoSpec.rgb = albedo;
    gAlbedoSpec.a = 1.0f;
}