#version 330 core

uniform sampler2D tex0;
in vec2 inoutUV;

void main()
{
    // gl_FragDepth = gl_FragCoord.z;
    vec4 texColor = texture(tex0, inoutUV);
	if(texColor.a < 0.1)
        discard;
}
