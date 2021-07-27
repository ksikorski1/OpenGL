#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec4 inNormal;
layout( location = 2 ) in vec2 inUV;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

out vec4 inoutPos;
out vec2 inoutUV;
out vec3 inoutNormal;


void main()
{
	gl_Position = matProj * matView * matModel * inPosition;

	inoutPos = inPosition;
	inoutUV = inUV;
	inoutNormal = vec3(inNormal);

}
