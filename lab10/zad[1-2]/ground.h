#ifndef __CGROUND
#define __CGROUND

#define __ALTITUDE_ERROR	99999

// -------------------------------
class CTriangle
{
public:

	// wspolrzedne punktow trojkata
	// uwaga! os pionowa w tym wypadku to z
	glm::vec3 p[3];


	// rownanie plaszczyzny Ax + By + Cz + D = 0
	float A,B,C,D;

	CTriangle() { }
	CTriangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
	{
		p[0] = p1;
		p[1] = p2;
		p[2] = p3;

		calculateEquation();
	}

	// obliczanie wspolczynnikow rownania
	void calculateEquation(void)
	{
		glm::vec3 v1, v2;

		v1 = p[1] - p[0];
		v2 = p[2] - p[0];

		A = v1.y * v2.z - v1.z * v2.y;	 // A
		B = v1.z * v2.x - v1.x * v2.z;   // B
		C = v1.x * v2.y - v1.y * v2.x;   // C


		D = - (A*p[0].x + B*p[0].y + C*p[0].z);	// D

		if (C == 0)
		{
			printf("Uwaga! Trojkat pionowy.\n");
		}

	}

	// czy punkt p1 jest po lewej stronie odcinka p2-p3
	inline float sign (glm::vec2 p1, glm::vec3 p2, glm::vec3 p3)
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}

	// czy punkt jest wewnatrz trojkata rzutowanego na plaszczyzne xy
	inline bool isInside (glm::vec2 point)
	{
		float s1, s2, s3;
		bool has_neg, has_pos;

		s1 = sign(point, p[0], p[1]);
		s2 = sign(point, p[1], p[2]);
		s3 = sign(point, p[2], p[0]);

		has_neg = (s1 < 0) || (s2 < 0) || (s3 < 0);
		has_pos = (s1 > 0) || (s2 > 0) || (s3 > 0);

		return !(has_neg && has_pos);
	}

	// obliczamy wysokosc punktu w trojkacie
	inline float altitude(glm::vec2 point)
	{
		if (C) return -(A*point.x + B*point.y + D)/C;
		else { return __ALTITUDE_ERROR; }
	}

};



// ---------------------------------------
// Klasa reprezentujaca podloze
class CGround
{
public:

	int Number_of_triangles = 0;
	std::vector<CTriangle> triangles;

	// stworzenie struktury z listy trojkatow
	// na przyklad przy okazji ladowania z pliku OBJ
	void CreateFromOBJ(std::vector<glm::vec3> vert)
	{
		glm::vec3 p1,p2,p3;

		std::vector<glm::vec3>::iterator it = vert.begin();
		while (it != vert.end())
		{
			p1 = *it++;
			p2 = *it++;
			p3 = *it++;
			// uwaga ! zamiana wspolrzednych, tak aby z byl wysokoscia
			triangles.push_back(CTriangle( glm::vec3(p1.x, p1.z, p1.y), glm::vec3(p2.x, p2.z, p2.y), glm::vec3(p3.x, p3.z, p3.y))  );

		}

		Number_of_triangles = triangles.size();
		printf("Created CGround of %d triangles.\n", Number_of_triangles);
	}

	// ----------------------------------------
	// Glowna funkcja obliczajaca wysokosci nad podlozem w punkcie X Z
	// - przeszukanie po wszystkich trojkatach
	// - gdy zostanie znaleziony trojkat, obliczana jest wysokosc Y
	float getAltitute(glm::vec2 point)
	{

		// obliczenie listy trojkatow nad ktorymi jestesmy
		CTriangle *collTriangle = NULL;

		for (auto it = triangles.begin(); it != triangles.end(); it++)
		{
			if ((*it).isInside(point))
			{
			    printf("znalazlem %d \n", it);
				collTriangle = &(*it);
				break;
			}
		}

		// jesli zostal znaleziony
		if (collTriangle)
		{
			return collTriangle->altitude(point);
		}
		else
		{
			printf("Brak ziemi pod nogami!\n");
			return 0.0;
		}
	}

};




#endif
