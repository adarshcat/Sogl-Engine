#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 texCoord;
in vec3 fragPosView;
in vec3 normal;

uniform vec3 albedo;

void main() {
    gPosition = fragPosView;
    gNormal = normalize(normal);
    gAlbedoSpec.rgb = albedo;
    gAlbedoSpec.a = 1.0f;
}