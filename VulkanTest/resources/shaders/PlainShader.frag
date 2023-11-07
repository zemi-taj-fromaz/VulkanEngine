#version 460

layout(location = 0) in vec3 fragColor;
layout (location = 1) in vec2 texCoord;

layout(set = 0, binding = 1) uniform  SceneData{
    vec4 fogColor; // w is for exponent
	vec4 fogDistances; //x for min, y for max, zw unused.
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
} sceneData;


layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(texCoord.x,texCoord.y,0.5f,1.0f);
    //outColor = vec4(fragColor + sceneData.ambientColor.xyz, 1.0f);
}