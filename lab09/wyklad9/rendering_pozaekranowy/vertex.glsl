#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;

uniform mat4 matProjView;

out vec4 inoutPos;
out vec2 inoutUV;


void main()
{
	gl_Position = matProjView * inPosition;

	inoutPos = inPosition;
	inoutUV = inUV;
}
