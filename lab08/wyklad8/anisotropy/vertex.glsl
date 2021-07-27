#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inColor;
layout( location = 2 ) in vec2 inUV;

uniform mat4 matProjView;

out vec2 inoutUV;


void main()
{
	gl_Position = matProjView * inPosition;

	inoutUV = inUV;
}
