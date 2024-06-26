#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 position;
layout(location = 3) out vec4 cameraPos;
layout(location = 4) out vec2 texCoord;

layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
    vec4 pos;
} camera;

layout(set = 2, binding = 0) uniform Resolution{
	vec2 res;
} resolution;

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


void main() {
    mat4 model = objectBuffer.objects[gl_InstanceIndex].model;
    mat4 MVP =  camera.proj * camera.view * model;
    //mat4 MVP = model;
    gl_Position =  MVP * vec4(inPosition, 1.0);
    fragColor = objectBuffer.objects[gl_InstanceIndex].color.xyz;
   // normal = inPosition - camera.pos.xyz;
	position = inPosition;
	cameraPos = camera.pos;

	
    float aspectRatio = resolution.res.x / resolution.res.y;
	texCoord = inPosition.xy;
	//texCoord.y -= 1.0;
	//texCoord.x *= aspectRatio;
}