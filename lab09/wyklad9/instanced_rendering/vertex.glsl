#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;
layout( location = 2 ) in vec3 inNormal;
layout( location = 3 ) in mat4 matModelInst;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

out vec4 inoutPos;
out vec2 inoutUV;

uniform float Fly;

void main()
{
    vec4 newPos = inPosition;
    newPos.y += Fly;
    gl_Position = matProj * matView * matModelInst * newPos;

    inoutPos = inPosition;
    inoutUV = inUV;
}
