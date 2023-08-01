#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
	mat4 normalMatrix ;
	vec4 lightPos ;
} ubo;
   
layout (location = 0 ) in vec3 color;
layout (location = 1 ) in vec3 VertexNormal;
layout (location = 2 ) in vec3 position;

// Instanced attributes
layout (location = 3) in vec3 instancePos;
layout (location = 4) in vec3 instanceRot;
layout (location = 5) in int instanceTexIndex;

layout (location = 0 ) out vec3 fragColor;
layout (location = 1 ) out vec3 Normal;
layout (location = 2 ) out vec4 Position; // adding position, so we know where we are
layout (location = 3 ) out vec3 LightPos;
layout (location = 4 ) out vec3 NormalView;

void main() {
								
	mat3 normalView = transpose ( inverse ( mat3 ( ubo.view * ubo.model)));
	Normal = normalView *  VertexNormal;

	LightPos = vec3( ubo.view * ubo.model * ubo.lightPos);
    fragColor = color;

	vec4 pos = vec4(position + instancePos, 1.0);
	vec4 glPosition = ubo.proj * ubo.view * ubo.model * pos;
	gl_Position = glPosition ;
	Position = ubo.view * ubo.model * pos ;
}