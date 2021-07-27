#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;
layout( location = 2 ) in vec2 inUv;
layout( location = 3 ) in mat4 matModelInst;


uniform mat4 matProj;
uniform mat4 matView;

uniform mat4 lightProj;
uniform mat4 lightView;


out vec4 ourPos;
out vec4 ourPosLight;
out vec3 ourNormal;
out vec2 inoutUV;

uniform float Fly;

void main()
{
    vec4 newPos = inPosition;
    newPos.y -= Fly;

	gl_Position = matProj * matView * matModelInst * newPos;


	ourPos = newPos;
	ourPosLight = lightProj * lightView * matModelInst * newPos;
	ourNormal = inNormal;
	inoutUV = inUv;

}
