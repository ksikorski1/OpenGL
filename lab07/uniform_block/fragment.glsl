#version 330

// Odebranie danych z vertex shadera
in VS_OUT
{
	vec4 position;
	vec4 color;
	vec4 normal;
	vec2 uv;

} fs_in;

// Ostateczny kolor fragmentu
out vec4 outColor;

// ------------------------------------
// Zmienne i bloki typu uniform
// ------------------------------------


// Pojedyncza zmienna uniform
uniform float Gray;

// Tablica zmiennych
uniform float Table_of_Numbers[11];


// Struktura
struct SColor
{
	float r;
	float g;
	float b;
	float a;
};

// Zmienna strukturalna typu uniform
uniform SColor greenColor;

struct StrColor
{
	vec4 color;
};

// Block zmiennych uniform
layout (std140) uniform Table_of_Colors
{
	StrColor Block_Color[6];

};



void main()
{
	vec4 newColor = fs_in.color;

	newColor.r = Gray;
	newColor.g = Table_of_Numbers[gl_PrimitiveID];

	//newColor.r = Table_of_Numbers[5];
	//newColor.g = Table_of_Numbers[5];
	//newColor.b = Table_of_Numbers[5];

	//newColor = vec4(greenColor.r, greenColor.g, greenColor.b, greenColor.a);

	// blok zmiennych uniform
	newColor = Block_Color[gl_PrimitiveID].color;



    outColor = newColor;
}
