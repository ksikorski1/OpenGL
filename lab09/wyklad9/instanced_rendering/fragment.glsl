#version 150 core

in vec4 inoutPos;
in vec2 inoutUV;

out vec4 outColor;

uniform sampler2D tex0;

void main()
{

    vec2 newUV = inoutUV;
    outColor = texture( tex0, newUV );

}
