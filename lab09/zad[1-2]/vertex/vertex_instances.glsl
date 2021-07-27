#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec4 inNormal;
layout( location = 2 ) in vec2 inUV;
layout( location = 3 ) in mat4 matModelInst;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat3 matNormal;

out vec3 myPosition;
out vec3 myNormal;
out vec2 inoutUV;

uniform float Fly;

void main()
{

    vec4 newPos = inPosition;
    newPos.y -= Fly;

	gl_Position = matProj * matView * matModelInst * newPos;

	myPosition = vec3(matModelInst * newPos);
	myNormal = matNormal * vec3(inNormal);
    inoutUV = inUV;

}

