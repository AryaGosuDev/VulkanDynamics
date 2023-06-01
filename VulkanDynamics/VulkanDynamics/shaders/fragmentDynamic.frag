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

layout (location = 0 ) in vec3 fragColor;
layout (location = 1 ) in vec3 Normal;
layout (location = 2 ) in vec4 Position;
layout (location = 3 ) in vec3 lightPos;
layout (location = 4 ) in vec3 NormalView;

layout (location = 0) out vec4 outColor;

void main() {
	vec3 lightDirectionView = normalize ( lightPos - vec3(Position));
	float lightDistance = length(lightPos - vec3(Position));

	// model how much light is available for this fragment's specular component
	float attenuation = 1.0f / (ufbo.ConstantAttenuation + 
								ufbo.LinearAttenuation * lightDistance + 
								ufbo.QuadraticAttenuation * lightDistance * lightDistance);
	// the direction of maximum highlight also changes per fragment
	vec3 halfVector = normalize(lightDirectionView + vec3( vec3(ufbo.EyeDirection) - vec3(Position) )  );

	float diffuse = max(0.0f, dot(Normal, lightDirectionView));
	float specular = pow ( max (dot (Normal, halfVector), 0.0f), ufbo.Shininess );

	// if diffuse == 0 -> specular = 0
	specular =  specular * max ( sign ( diffuse - 0.0f ) , 0.0f );

	vec3 scatteredLight = vec3(ufbo.Ambient + (ufbo.LightColor * diffuse)) ;
	vec3 reflectedLight = vec3(ufbo.LightColor * specular * attenuation);
	
	//vec3 rgb = min(fragColor.rgb * scatteredLight + reflectedLight ,vec3(1.0));

    //outColor = vec4 ( scatteredLight + reflectedLight , 1.0f );
	//outColor = vec4 ( scatteredLight  , 1.0f );
	outColor = vec4 ( 1.0f,1.0f,1.0f, 1.0f );
}