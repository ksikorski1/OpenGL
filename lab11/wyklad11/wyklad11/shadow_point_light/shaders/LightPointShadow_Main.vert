#version 330
layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

out vec4 ourPos;
out vec3 ourNormal;



void main()
{
	gl_Position = matProj * matView * matModel * inPosition;
	ourPos = matModel * inPosition;
	ourNormal = inNormal;

}
