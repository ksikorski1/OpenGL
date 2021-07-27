#version 150

in vec4 ourPos;
in vec4 ourPosLight;
in vec3 ourNormal;

struct LightParameters
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Direction;
};

// Potrzebne do shadowMap i oswietlenia kierunkowego
uniform sampler2D shadowMap;
uniform vec3 Camera_Position;
uniform vec3 Light_Direction;


out vec4 outColor;



// ---------------------------------------------------------------------------
// Zwraca [0-1], gdzie 1 oznacza ze fragment jest calkowicie w cieniu
// ---------------------------------------------------------------------------
float Calculate_Shadow(vec4 fragPosLight, vec3 LightDirection)
{

	// Debug: sprawdzenie zasiegu cienia
	//return 0;

	// Korekcja perspektywiczna (dla oswietlenia kierunkowego niepotrzebna)
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Brak cienia poza zasiegiem "kamery cienia" patrzac w dal
    //if(projCoords.z > 1.0)
    //    return 0.0;


    // pobranie z tekstury shadowMap odleglosci od swiatla fragmentu oswietlonego
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // obliczenie aktualnej odleglosci od swiatla
    float currentDepth = projCoords.z;

    // Metoda 1: proste sprawdzenie czy fragment jest w cieniu po odleglosci
	//return (currentDepth > closestDepth) ? 1.0 : 0.0;


	// -------------------------------------------
	// Efekty uboczne: Shadow Acne i Peter Panning
	// -------------------------------------------

	//bias mowi o artefaktach - paskach - dobrac eksperymentalnie
	float bias = 0;
    bias = max(0.004 * (1.0 - dot(ourNormal, Light_Direction)), 0.001);

	// Metoda 2: usuniecie shadow acne
	//return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;



	// Metoda 3: Smooth filtering
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(float x = -1; x <= 1; x+=1.0)
	{
		for(float y = -1; y <= 1; y+=1.0)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;


	return shadow;
}


// ---------------------------------------------------------------------------
// Uproszczone oswietlenie kierunkowe ktore UWAGA: nie zwraca skladowej ambient
// ---------------------------------------------------------------------------
vec3 Calculate_DirectionalLight(struct LightParameters light, vec3 CameraPosition)
{

	// diffuse
	vec3  lightDirection = normalize(light.Direction);
	float diffuseCoeff = max(dot(ourNormal, -light.Direction), 0.0);
	vec3  resultDiffuse = diffuseCoeff * vec3(light.Diffuse);
	vec3  diffusePart = resultDiffuse;

	// specular
	vec3  viewDir = normalize(CameraPosition - vec3(ourPos));
	vec3  reflectDir = reflect(light.Direction, ourNormal);
	float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3  specularPart = specularCoeff * vec3(light.Specular);

	// result
	return (diffusePart + specularPart);
}



// ---------------------------------------------------------------------------
void main()
{

	LightParameters myLight;

	// Parametry swiatla
	myLight.Ambient = vec3(0.2, 0.2, 0.2);
	myLight.Diffuse = vec3(0.8, 0.8, 0.8);
	myLight.Specular = vec3(0.5, 0.5, 0.5);
	myLight.Direction = Light_Direction;


	// Kolor wyjsciowy
	vec3  myColor = vec3(0.1, 1.0, 0.1);

	// Oswietlenie kierunkowe
	vec3  lightPart = Calculate_DirectionalLight(myLight, Camera_Position);

	// Cienie
	float shadowPart = Calculate_Shadow(ourPosLight, Light_Direction);

	// Kolor finalny
	vec3  finalColor = (myLight.Ambient + (1 - shadowPart) * lightPart) * myColor;

	outColor = vec4(finalColor , 1.0);

}
