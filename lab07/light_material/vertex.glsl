#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec4 inNormal;

out vec3 myPosition;
out vec3 myNormal;

uniform mat4 matProjViewModel;

void main()
{

	gl_Position = matProjViewModel * inPosition;

	myPosition = vec3(inPosition);
	myNormal = vec3(inNormal);

}

