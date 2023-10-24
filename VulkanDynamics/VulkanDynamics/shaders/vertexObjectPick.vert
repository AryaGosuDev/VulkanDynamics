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
layout (location = 6) in vec3 instanceColor;

layout(location = 0) out uint outObjectID;

void main() {
    vec4 pos = vec4(position + instancePos, 1.0);

    gl_Position = ubo.proj * ubo.view * ubo.model * pos;
    outObjectID = uint(instanceColor.z);
}