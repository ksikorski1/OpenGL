#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

// Dane odebrane z vertex shadera
in vData
{
    vec2 UV;
	vec3 Normal;

} vertex[];

// Dane wyslane do fragment shadera
out fData
{
    vec2 UV;
    vec3 Pos;
	vec3 Normal;

} frag;

// Macierze rzutowania
uniform mat4 matPVArray[6];


void main()
{
    for(int face = 0; face < 6; ++face)
    {
		// wskazanie warstwy framebuffera (sciany cubemapy)
        gl_Layer = face; 
		
        for(int i = 0; i < 3; ++i) 
        {
            // Uzupelnienie danych przeslanych do fragment shadera
			frag.Pos = gl_in[i].gl_Position.xyz;
            frag.UV = vertex[i].UV;
			frag.Normal = vertex[i].Normal;
			
			// Obliczenie finalnego polozenia wierzcholka
			// Korzystamy z macierzu rzutowania
            gl_Position = matPVArray[face] * gl_in[i].gl_Position;
			
			// Tworzymy wierzcholek
            EmitVertex();
        }
		
		// Tworzymy prymityw (tutaj: trojkata po kazdych trzech wierzcholkach)
        EndPrimitive();
    }
}
