#version 150 core

in vec4 fragPos;
out vec4 outColor;

uniform samplerCube tex0;

void main()
{
	outColor = texture(tex0, fragPos.xyz);
}
