#version 150 core

in vec3 myPosition;
in vec3 myNormal;
in vec2 inoutUV;

out vec4 outColor;

// -----------------------
// Kamera
uniform vec3 Camera_Position;
uniform sampler2D tex0;

// -----------------------
// Oswietlenie
struct SLight
{
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
    vec4 Position;
    float Attenuation;
};

uniform struct SLight myLight;


// -----------------------
// Materialy
struct SMaterial
{
	vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
    float Shininess;
};

uniform struct SMaterial myMaterial;


// ---------------------------------------------------------------------------
vec4 Calculate_PointLight(vec3 fragPos, vec3 fragNormal, struct SLight myLight, struct SMaterial myMaterial)
{

	// Distance light
	float Light_dist = length(vec3(myLight.Position) - fragPos);
	float Attenuation = 1.0 / (1.0 + myLight.Attenuation * (Light_dist * Light_dist));

	// ---------------------------
	// Ambient
	vec4 ambientPart = myLight.Ambient * myMaterial.Ambient * Attenuation;


	// ---------------------------
	// Diffuse

	// Obliczenie wektora (swiatlo - wierzcholek)
	// czyli kierunku padania swiatla na wierzcholek
	vec3 lightDirection = normalize(vec3(myLight.Position) - fragPos);

	// obliczenie kata pomiedzy wektorem lightDir oraz wektorem normalnym
	// wartosc kata okresla pod jakim katem padaja promienie
	float lightCoeff = max(dot(fragNormal, lightDirection), 0.0);

	vec4 diffusePart = lightCoeff * myLight.Diffuse * myMaterial.Diffuse * Attenuation;


	// ------------------
	// Specular
	vec3 viewDir = normalize(Camera_Position - fragPos);
	vec3  reflectDir = reflect(lightDirection, fragNormal);
	// obliczanie wspolczynnika specular z parametrem shininess
	float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), myMaterial.Shininess);
	vec4  specularPart = specularCoeff * myLight.Specular * myMaterial.Specular * Attenuation;

	// -----------------
	// Ostateczny
	return (ambientPart + diffusePart + specularPart);
}


void main()
{

	// Kolor startowy
	//vec4 objectColor = myMaterial.Diffuse; //vec3(abs(sin(length(myPosition))), 1.0, 1.0);
    //vec4 objectColor = texture( tex0, inoutUV );
    vec4 objectColor = vec4(0.4, 0.6, 0.7, 1.0);
	// Obliczanie swiatel punktowych
	vec4 lightPart = Calculate_PointLight(myPosition, myNormal, myLight, myMaterial);

	// Ostateczny kolor fragmentu
	vec4 finalColor = lightPart * objectColor;

	outColor = finalColor;

}
