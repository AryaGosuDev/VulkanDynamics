#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
	mat4 normalMatrix ;
	vec4 lightPos ;
} ubo;

layout(set = 0, binding = 2) uniform UniformBufferDynamicObject {
    mat4 model;
} uboDyn;
   
layout (location = 0 ) in vec3 color;
layout (location = 1 ) in vec3 VertexNormal;
layout (location = 2 ) in vec3 position;

layout (location = 0 ) out vec3 fragColor;
layout (location = 1 ) out vec3 Normal;
layout (location = 2 ) out vec4 Position; // adding position, so we know where we are
layout (location = 3 ) out vec3 LightPos;
layout (location = 4 ) out vec3 NormalView;

void main() {

	//mat4 translateToCenter = mat4 ( 1.0, 0.0, 0.0, 0.0,0.0, 1.0, 0.0, -1.5,0.0, 0.0, 1.0, 0.22,0.0, 0.0, 0.0, 1.0 ) ;
	mat4 translateToCenter = mat4 ( 1.0, 0.0, 0.0, -9.0, 0.0, 1.0, 0.0, -9.0, 0.0, 0.0, 1.0, -30.0,0.0, 0.0, 0.0, 1.0 ) ;
								
	Normal = normalize( mat3(ubo.normalMatrix) *  VertexNormal);
	//Normal = normalize( mat3(transpose(inverse(ubo.view * uboDyn.model ))) *  VertexNormal);
	LightPos = vec3( ubo.view * ubo.lightPos);
    fragColor = color;
	//Position = ubo.view * uboDyn.model * transpose(translateToCenter) * vec4(position, 1.0f);
	vec4 glPosition = ubo.proj * ubo.view * uboDyn.model * transpose(translateToCenter) * vec4(position, 1.0f);
	

	const vec3 positions[3] = vec3[3](
		vec3(1.f,1.f, 0.0f),
		vec3(-1.f,1.f, 0.0f),
		vec3(0.f,-1.f, 0.0f)
	);

	//gl_Position = vec4(positions[gl_VertexIndex], 1.0f);


	gl_Position = glPosition ;
}