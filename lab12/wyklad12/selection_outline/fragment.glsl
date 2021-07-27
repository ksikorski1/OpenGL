#version 150

in vec4 inoutPos;
in vec2 inoutUV;

out vec4 outColor;

uniform sampler2D tex0;
uniform int selected = 0;

void main()
{
	outColor = texture( tex0, inoutUV );

    if (selected == 1)
    {
        outColor = texture( tex0, inoutUV ) * vec4(0.3, 0.3, 0.5, 1.0);
    }
}
