#version 330 core

layout (location = 0) out vec4 gNormalMet;
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
uniform float roughness;
uniform float metallic;

void main() {
    gNormalMet.rgb = normalize(normal);
    gNormalMet.a = metallic;
    gAlbedoSpec.rgb = albedo;
    gAlbedoSpec.a = roughness;
}