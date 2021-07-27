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

uniform SLight myLight;


// -----------------------
// Materialy
struct SMaterial
{
	vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
    float Shininess;
};

uniform SMaterial myMaterial;


// ---------------------------------------------------------------------------
vec3 Calculate_PointLight(vec3 fragPos, vec3 fragNormal, SLight light, SMaterial material)
{

	// Distance light
	float Light_dist = length(vec3(light.Position) - fragPos);
	float Attenuation = 1.0 / (1.0 + light.Attenuation * (Light_dist * Light_dist));

	// ---------------------------
	// Ambient
	vec3 ambientPart = vec3(light.Ambient) * vec3(material.Ambient) * Attenuation;


	// ---------------------------
	// Diffuse

	// Obliczenie wektora (swiatlo - wierzcholek)
	// czyli kierunku padania swiatla na wierzcholek
	vec3 lightDirection = normalize(vec3(light.Position) - fragPos);

	// obliczenie kata pomiedzy wektorem lightDir oraz wektorem normalnym
	// wartosc kata okresla pod jakim katem padaja promienie
	float lightCoeff = max(dot(fragNormal, lightDirection), 0.0);

	vec3 diffusePart = lightCoeff * vec3(light.Diffuse) * vec3(material.Diffuse) * Attenuation;


	// ------------------
	// Specular
	vec3 viewDir = normalize(Camera_Position - fragPos);
	vec3  reflectDir = reflect(-lightDirection, fragNormal);
	// obliczanie wspolczynnika specular z parametrem shininess
	float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);
	vec3  specularPart = specularCoeff * vec3(light.Specular) * vec3(material.Specular) * Attenuation;

	// -----------------
	// Ostateczny
	return (ambientPart + diffusePart + specularPart);
}


void main()
{

	// Kolor startowy
	//vec4 objectColor = myMaterial.Diffuse; //vec3(abs(sin(length(myPosition))), 1.0, 1.0);
    //vec4 objectColor = texture( tex0, inoutUV );
    vec4 texColor = texture(tex0, inoutUV);
	if(texColor.a < 0.1)
        discard;
	// Obliczanie swiatel punktowych
	vec4 lightPart = vec4(Calculate_PointLight(myPosition, myNormal, myLight, myMaterial),1.0);

	// Ostateczny kolor fragmentu
	vec4 finalColor = lightPart * texColor;

	outColor = finalColor;
}
