#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec2 texCoord;
layout(location = 3) out vec3 cameraPos;
layout(location = 4) out vec4 fragPos;

layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
    vec4 pos;
} camera;

//push constants block

struct ObjectData
{
    mat4 model;
    vec4 color;
};

//all object matrices
layout(std140, set = 1, binding = 0) buffer ObjectBuffer{

	ObjectData objects[];
} objectBuffer;

layout(set = 4, binding = 0) uniform AmplitudeBufferObj {
	float amplitude;
	float choppy;
} abo;

layout(set = 5, binding = 0, rgba32f) uniform readonly image2D heightmap;
layout(set = 6, binding = 0, rgba32f) uniform readonly image2D normalmap;

void main() {
    mat4 model = objectBuffer.objects[gl_InstanceIndex].model;
    mat4 MVP =  camera.proj * camera.view * model;
	
	vec4 Displacement = imageLoad(heightmap, ivec2(inTexCoord));
	Displacement.y *= abo.amplitude;
	
	vec4 slope = imageLoad(normalmap, ivec2(inTexCoord));
	
	fragPos.xyz = inPosition + Displacement.xyz;
	fragPos.w = Displacement.w;
	
	//vec3 Position = vec3(inPosition.x, inPosition.y + imageLoad(heightmap, ivec2(inTexCoord)).x, inPosition.z);
    gl_Position =  MVP * vec4(fragPos.xyz, 1.0f);
    texCoord = inTexCoord;
	
	//vec4 worldCoord = model * vec4(fragPos.xyz,1.0);
    //fragPos.xyz = worldCoord.xyz / worldCoord.w;
	
    fragColor = objectBuffer.objects[gl_InstanceIndex].color.xyz;
	
	normal = normalize(vec3(
        - ( slope.x / (1.0f + abo.choppy * slope.z) ),
        1.0f,
        - ( slope.y / (1.0f + abo.choppy * slope.w) )
    ));
	
	cameraPos = camera.pos.xyz;
}