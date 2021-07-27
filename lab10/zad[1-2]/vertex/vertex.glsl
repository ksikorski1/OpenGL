#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec4 inNormal;
layout( location = 2 ) in vec2 inUV;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;
uniform mat3 matNormal;

out vec3 myPosition;
out vec3 myNormal;
out vec2 inoutUV;

void main()
{

	gl_Position = matProj * matView * matModel * inPosition;

	myPosition = vec3(matModel * inPosition);
	myNormal = matNormal * vec3(inNormal);
    inoutUV = inUV;

}

