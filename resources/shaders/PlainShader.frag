#version 460

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 normal;

layout(set = 1, binding = 0) uniform  SceneData{
    vec4 fogColor; // w is for exponent
	vec4 fogDistances; //x for min, y for max, zw unused.
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
	vec4 sunPosition;
} sceneData;


layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor,1.0f);
}