// Fragment shader
#version 330


// Predefiniowane dane wejsciowe
//in  vec4 gl_FragCoord;
//in  int  gl_PrimitiveID;


// Dane wyjsciowe
out vec4 outColor;

in vec3 fragColor;

uniform float Jasnosc;

float random (vec2 uv)
{
	return sin(dot(uv,vec2(12.9898,78.233)))*43758.5453123
	  - floor(sin(dot(uv,vec2(12.9898,78.233)))*43758.5453123);
}

void main()
{
	if (gl_PrimitiveID%2 == 0)
		outColor = vec4(0.8 * random(gl_FragCoord.xy), random(gl_FragCoord.xy), 0.8 * random(gl_FragCoord.yx), 1.0);
	else
		outColor = vec4(random(gl_FragCoord.xy), 0.7*random(gl_FragCoord.xy), random(gl_FragCoord.xy), 1.0);

    outColor.r = (gl_FragCoord.x/600.0f);

    outColor.b = (gl_FragCoord.y/600.0f);

    outColor += vec4( vec3(Jasnosc), 0.0 );
}


/*
void main()
{

	//outColor = vec4(0.0, 1.0, 0.0, 1.0);

	//outColor.r = (gl_FragCoord.x/600.0f)*2;

	//outColor.b = (gl_PrimitiveID%2);

	outColor = vec4(fragColor, 1.0);

	outColor += vec4( vec3(Jasnosc), 0.0 );
}
*/
