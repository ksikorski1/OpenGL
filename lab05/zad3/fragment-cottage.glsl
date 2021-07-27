#version 150

in vec4 inoutPos;
out vec4 outColor;
in vec3 inoutColor;
in vec2 inoutUV;

uniform sampler2D tex0;
void main()
{
	outColor = texture( tex0, inoutUV );
}
