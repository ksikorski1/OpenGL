#version 330 core
layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;
layout( location = 2 ) in vec2 inUv;
layout( location = 3 ) in mat4 matModelInst;

uniform mat4 lightProj;
uniform mat4 lightView;

uniform mat4 matModel;

out vec2 inoutUV;

uniform float Fly;

void main()
{
    vec4 newPos = inPosition;
    newPos.y -= Fly;

    gl_Position = lightProj * lightView * matModelInst * inPosition;
    inoutUV = inUv;
}
