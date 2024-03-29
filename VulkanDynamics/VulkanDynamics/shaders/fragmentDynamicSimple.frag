#version 450

layout(set = 0, binding = 1) uniform UniformFragBufferObject {
	vec4 Ambient;
	vec4 LightColor;
	float Shininess;
	float Strength;
	vec4 EyeDirection;
	float ConstantAttenuation; // attenuation coefficients
	float LinearAttenuation;
	float QuadraticAttenuation;
	mat4 viewMatrix ;
	mat4 eyeViewMatrix;
} ufbo;

layout(set = 0, binding = 2) uniform sampler2D reflectSampler;

layout(push_constant) uniform PushConstants {
    bool useReflectionSampler;
} pc;

layout (location = 0 ) in vec3 fragColor;
layout (location = 1 ) in vec3 Normal;
layout (location = 2 ) in vec4 Position;
layout (location = 3 ) in vec3 lightPos;
layout (location = 4 ) in vec3 NormalView;
layout (location = 5 ) in vec2 texCoords;

layout (location = 0) out vec4 outColor;

void main() {
	vec3 lightDirectionView = normalize ( lightPos - vec3(Position));

	float diffuse = max(0.0f, dot(Normal, lightDirectionView));
	//vec3 scatteredLight = vec3(ufbo.LightColor * diffuse) ;
	vec3 scatteredLight = fragColor * diffuse ;

	if (pc.useReflectionSampler) {
		outColor = texture(reflectSampler, texCoords);
	} else {
		outColor = vec4(scatteredLight, 1.0f);
	}
}