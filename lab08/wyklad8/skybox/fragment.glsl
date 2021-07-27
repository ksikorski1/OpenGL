#version 150 core

in vec4 inoutPos;
in vec2 inoutUV;
in vec3 inoutNormal;


out vec4 outColor;

uniform vec3 Camera_Position;

uniform samplerCube tex_cube;
uniform sampler2D tex0;

void main()
{
    vec3 Dir = normalize(inoutPos.xyz - Camera_Position);

    vec3 ReflectionDir = reflect(Dir, inoutNormal);
    vec4 reflectCubeMap = texture(tex_cube, ReflectionDir);

    vec4 tekstura = texture(tex0, inoutUV);

    outColor = mix(tekstura, reflectCubeMap, 0.5);


    //vec3 RefractionDir = refract(Dir, inoutNormal, 1.0/1.2);
    //vec4 refractColor = texture (tex_cube, RefractionDir);

    //outColor = reflectCubeMap;

	//outColor = texture(tex0, inoutUV);
}
