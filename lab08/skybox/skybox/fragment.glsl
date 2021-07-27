#version 150 core

in vec4 inoutPos;
in vec2 inoutUV;
in vec3 inoutNormal;

out vec4 outColor;

uniform sampler2D tex0;
uniform samplerCube skybox;

uniform vec3 Camera_Position;

void main()
{
	//outColor = texture(tex0, inoutUV);
	vec4 BaseColor = texture(tex0, inoutUV);


	vec3 Dir = normalize(inoutPos.rgb - Camera_Position);
	vec3 Reflection = reflect(Dir, inoutNormal);

	vec4 ReflectionColor = texture(skybox, Reflection);
	outColor = ReflectionColor;



    //outColor = mix(BaseColor, ReflectionColor, 0.5);
}
