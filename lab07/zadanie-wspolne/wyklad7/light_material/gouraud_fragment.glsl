#version 150 core

in vec4 ourColor;
in vec2 inoutUV;

out vec4 outColor;
uniform sampler2D tex0;

void main()
{
	vec4 objectColor = texture( tex0, inoutUV );

	vec4 result = ourColor * objectColor;

	outColor = result;

}
