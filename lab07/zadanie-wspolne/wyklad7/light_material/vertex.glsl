#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec4 inNormal;
layout( location = 2 ) in vec2 inUV;

out vec3 myPosition;
out vec3 myNormal;
out vec2 inoutUV;

uniform mat4 matProjViewModel;
uniform mat4 matModel;
uniform mat3 matNormal;

void main()
{

	gl_Position = matProjViewModel * inPosition;

	myPosition = vec3(matModel * inPosition);
	myNormal = matNormal * vec3(inNormal);
    inoutUV = inUV;

}

