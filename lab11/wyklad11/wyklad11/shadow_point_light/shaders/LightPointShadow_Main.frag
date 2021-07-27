// ---------------------------------------------------------------------------
// https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
// ---------------------------------------------------------------------------

#version 150 core

in vec4 ourPos;
in vec3 ourNormal;

out vec4 outColor;

// ---------------------------------------------------------------------------
// Zmienne i parametry oswietlenia

uniform samplerCube ShadowMapCube;

uniform float FarPlane;
uniform vec3  CameraPosition;
uniform vec3  LightPosition;

vec3 LightAmbient = vec3(0.2, 0.2, 0.2);
vec3 LightDiffuse = vec3(0.8, 0.8, 0.8);
vec3 LightSpecular = vec3(0.5, 0.5, 0.5);

// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------
// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);



// ---------------------------------------------------------------------------
// Reference: https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
float Calculate_ShadowPoint(vec3 fragPos)
{
	// get vector between fragment position and light position
    vec3 fragToLight = fragPos - LightPosition;

	// now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.04;

    /*
    // use the light to fragment vector to sample from the depth map
    float closestDepth = texture(shadowMapCube, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= farPlane;

    // now test for shadows
    float bias = 0.04;
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
	*/

    int samples = 20;
    float viewDistance = length(CameraPosition - fragPos.xyz);
    float diskRadius = (1.0 + (viewDistance / FarPlane)) / 25.0;

    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(ShadowMapCube, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= FarPlane;   // undo mapping [0;1]

        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    shadow /= float(samples);

    return shadow;
}



// ---------------------------------------------------------------------------
vec3 Calculate_PointLight(vec3 fragPos, vec3 fragNormal)
{

	// Distance light
	float Light_dist = length(LightPosition - fragPos);
	float Attenuation = 1.0 / (1.0 + 0.01f * (Light_dist));

	// ---------------------------
	// Ambient
	// vec4 ambientPart = myLight.Ambient * Attenuation;

	// ---------------------------
	// Diffuse

	// Obliczenie wektora (swiatlo - wierzcholek)
	// czyli kierunku padania swiatla na wierzcholek
	vec3 lightDir = normalize(LightPosition - fragPos);

	// obliczenie kata pomiedzy wektorem lightDir oraz wektorem normalnym
	// wartosc kata okresla pod jakim katem padaja promienie
	float lightCoeff = max(dot(fragNormal, lightDir), 0.0);
	vec3 diffusePart = lightCoeff * LightDiffuse * Attenuation;


	// ------------------
	// Specular
	vec3 viewDir = normalize(CameraPosition - fragPos);
	vec3  reflectDir = reflect(-lightDir, fragNormal);
	// obliczanie wspolczynnika specular z parametrem shininess
	float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3  specularPart = specularCoeff * LightSpecular * Attenuation;

	// -----------------
	// Ostateczny
	return (diffusePart + specularPart);
}



// ---------------------------------------------------------------------------
void main()
{


	// Kolor wyjsciowy
	vec3  myColor = vec3(0.1, 1.0, 0.1);

	// Oswietlenie kierunkowe
	vec3  lightPart = Calculate_PointLight(ourPos.xyz, ourNormal);

	// Cienie
	float shadowPart = Calculate_ShadowPoint(ourPos.xyz);

	// Kolor finalny
	vec3  finalColor = (LightAmbient + (1 - shadowPart) * lightPart) * myColor;

	outColor = vec4(finalColor, 1.0);

}
