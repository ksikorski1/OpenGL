#version 330 core
layout (location = 0) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;
layout( location = 2 ) in vec2 inUvs;

uniform mat4 lightProj;
uniform mat4 lightView;

uniform mat4 matProjViewModel;
uniform mat4 matModel;
uniform mat3 matNormal;
out vec2 inoutUV;

void main()
{
    //gl_Position = lightProj * lightView * inPosition;
    gl_Position = lightProj * lightView * matModel * inPosition;
	inoutUV = inUvs;
}
