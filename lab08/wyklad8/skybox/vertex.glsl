#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;
layout( location = 2 ) in vec3 inNormal;

uniform mat4 Matrix_proj_view;

out vec4 inoutPos;
out vec2 inoutUV;
out vec3 inoutNormal;


void main()
{
	gl_Position = Matrix_proj_view * inPosition;

	inoutPos = inPosition;
	inoutUV = inUV;
	inoutNormal = inNormal;

}
