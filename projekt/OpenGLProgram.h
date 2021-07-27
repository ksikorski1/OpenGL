#ifndef __OpenGLProgram
#define __OpenGLProgram

#include <tuple>


class OpenGLProgram{
public:
    GLuint program;
    GLuint DepthMap_FrameBuffer;
    GLuint DepthMap_Texture;

    const unsigned int DepthMap_Width = 1024, DepthMap_Height = 1024;

    void CreateProgram(){
        program = glCreateProgram();
    }

    void ActivateProgram(){
        glUseProgram(program);
    }

    void DeactiveProgram(){
        glUseProgram(0);
    }

    GLuint SendProgramId(){
        return program;
    }

    void LoadShaders(char* vertex, char* fragment){
        glAttachShader( program, LoadShader(GL_VERTEX_SHADER, vertex));
        glAttachShader( program, LoadShader(GL_FRAGMENT_SHADER, fragment));

        LinkAndValidateProgram( program );
    }

    void CreateShadowMap()
    {
        // ---------------------------------------
        // NOWE: Shadow mapping
        // ---------------------------------------

        // 1. Stworzenie obiektu tekstury
        glGenTextures(1, &DepthMap_Texture);
        glBindTexture(GL_TEXTURE_2D, DepthMap_Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DepthMap_Width, DepthMap_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // 2. Stworzenie obiektu ramki bufora jedynie z buforem glebokosci
        // Skladowa koloru nie jest nam potrzebna
        glGenFramebuffers(1, &DepthMap_FrameBuffer);

        // 3. Dolaczanie tekstury do ramki bufora
        glBindFramebuffer(GL_FRAMEBUFFER, DepthMap_FrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMap_Texture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ClearBuffer(){
        glViewport(0, 0, DepthMap_Width, DepthMap_Height);
        glBindFramebuffer(GL_FRAMEBUFFER, DepthMap_FrameBuffer);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    GLuint getDepthMapTexture(){
        return DepthMap_Texture;
    }
};

class OpenGLObject
{

public:
    GLuint idProg;
	std::vector<glm::vec3> OBJ_vertices;
	std::vector<glm::vec2> OBJ_uvs;
	std::vector<glm::vec3> OBJ_normals;
	glm::mat4 matModel = glm::mat4(1.0);
	glm::mat4 matProjViewMode;

    SMaterial ObjectMaterial;

	GLuint vArray;
	GLuint Size;

	GLuint TextureID;
	int tex_width;
    int tex_height;
    int n;
    unsigned char *data;

    GLuint skyboxTexture;

	OpenGLObject() { }

	void SetProgramId(GLuint program){
        idProg = program;
    }
    void LoadShaders(char* vertex, char* fragment){
        glAttachShader( idProg, LoadShader(GL_VERTEX_SHADER, vertex));
        glAttachShader( idProg, LoadShader(GL_FRAGMENT_SHADER, fragment));

        LinkAndValidateProgram( idProg );
    }

	void DrawWithSkyBox(){

        sendMatrices();
        sendCameraPosition();
        GLuint SkyBox_Texture = skyboxTexture;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, SkyBox_Texture);

        glUniform1i(glGetUniformLocation(idProg, "tex0"), 0);
        glUniform1i(glGetUniformLocation(idProg, "tex_cube"), 1);

        glBindVertexArray( vArray );
            glDrawArrays( GL_TRIANGLES, 0, Size );

        glBindVertexArray( 0 );
	}

	// ---------------------------------------
	void Load(char *filename)
	{

		if (!loadOBJ(filename, OBJ_vertices, OBJ_uvs, OBJ_normals))
		{
			printf("Not loaded!\n");
			exit(1);
		}

		// Liczba wierzcholkow potrzebna do wyrenderowania obiektu
		Size = OBJ_vertices.size();

		// Vertex arrays
		glGenVertexArrays( 1, &this->vArray );
		glBindVertexArray( this->vArray );

		// Wspolrzedne wierzchokow
		GLuint vBuffer_coord;
		glGenBuffers( 1, &vBuffer_coord );
		glBindBuffer( GL_ARRAY_BUFFER, vBuffer_coord );
		glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 0 );

		// Wektory normalne
		GLuint vBuffer_normal;
		glGenBuffers( 1, &vBuffer_normal );
		glBindBuffer( GL_ARRAY_BUFFER, vBuffer_normal );
		glBufferData( GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 1 );

        //uv
		GLuint vBuffer_uv;
		glGenBuffers( 1, &vBuffer_uv );
        glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv );
        glBufferData( GL_ARRAY_BUFFER, OBJ_uvs.size() * sizeof(glm::vec2), &OBJ_uvs[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 2 );

        __CHECK_FOR_ERRORS
		glBindVertexArray( 0 );

	}

	void sendMaterials() {

        // (a) material
		glUniform4fv( glGetUniformLocation(idProg, "myMaterial.Ambient"), 1, &ObjectMaterial.Ambient[0]);
		glUniform4fv( glGetUniformLocation(idProg, "myMaterial.Diffuse"), 1, &ObjectMaterial.Diffuse[0]);
		glUniform4fv( glGetUniformLocation(idProg, "myMaterial.Specular"), 1, &ObjectMaterial.Specular[0]);
		glUniform1f( glGetUniformLocation(idProg, "myMaterial.Shininess"), ObjectMaterial.Shininess);
    }

    void sendMatrices(){
        //matProjViewMode = matProj * matView * matModel;

		glm::mat3 matNormal = glm::transpose(glm::inverse(glm::mat3(matModel)));

		glUniformMatrix4fv( glGetUniformLocation( idProg, "matView" ), 1, GL_FALSE, &(matView[0][0]) );
        glUniformMatrix4fv( glGetUniformLocation( idProg, "matProj" ), 1, GL_FALSE, &(matProj[0][0]) );
		glUniformMatrix4fv( glGetUniformLocation( idProg, "matModel" ), 1, GL_FALSE, &(matModel[0][0]) );
		glUniformMatrix3fv( glGetUniformLocation( idProg, "matNormal" ), 1, GL_FALSE, &(matNormal[0][0]) );
    }

    void sendCameraPosition(){
        glUniform3fv( glGetUniformLocation( idProg, "Camera_Position" ), 1, &Camera_Position[0] );
    }

    void sendLightning(){

		glUniform4fv( glGetUniformLocation(idProg, "myLight.Ambient"), 1, &Light1.Ambient[0]);
		glUniform4fv( glGetUniformLocation(idProg, "myLight.Diffuse"), 1, &Light1.Diffuse[0]);
		glUniform4fv( glGetUniformLocation(idProg, "myLight.Specular"), 1, &Light1.Specular[0]);
		glUniform4fv( glGetUniformLocation(idProg, "myLight.Position"), 1, &Light1.Position[0]);
		glUniform1f( glGetUniformLocation(idProg, "myLight.Attenuation"), Light1.Attenuation);
    }
    void sendDirectionalLightning(){
        glUniformMatrix4fv( glGetUniformLocation( idProg, "lightProj" ), 1, GL_FALSE, glm::value_ptr(lightProj) );
		glUniformMatrix4fv( glGetUniformLocation( idProg, "lightView" ), 1, GL_FALSE, glm::value_ptr(lightView) );
    }

    void getMaterials(SMaterial Material){
        ObjectMaterial = Material;
    }

    void CreateTexture(char* filename){

        //stbi_set_flip_vertically_on_load(true);

        data = stbi_load(filename, &tex_width, &tex_height, &n, STBI_rgb_alpha);

        glGenTextures(1, &TextureID);
        glBindTexture(GL_TEXTURE_2D, TextureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        // GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // (b) Zachowanie tekstury w przypadku powiekszenia

        // GL_LINEAR, GL_NEAREST
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // (c) Zachowanie tekstury w przypadku pomniejszenia

        // GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST,
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

        // Generowanie mipmap automatycznie
        glGenerateMipmap(GL_TEXTURE_2D);

        // Podstawowy level mipmap (default 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    }
    void getSkyBoxTexture(GLuint skyboxTex){
        skyboxTexture =  skyboxTex;
    }
    void move(float X = 0.0f, float Y = 0.0f, float Z = 0.0f){
        matModel = glm::translate( matModel, glm::vec3( X, Y, Z) );
    }

    void rotateX(float angle){
        matModel = glm::rotate( matModel, angle, glm::vec3( 1.0f, 0.0f, 0.0f ) );
    }
    void rotateY(float angle){
        matModel = glm::rotate( matModel, angle, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    }
    void rotateZ(float angle){
        matModel = glm::rotate( matModel, angle, glm::vec3( 0.0f, 0.0f, 1.0f ) );
    }
    void scale(float X = 0.0f, float Y = 0.0f, float Z = 0.0f){
        matModel = glm::scale(matModel, glm::vec3(X, Y, Z));
    }


    void DrawToShadowMap(GLuint DepthMap_Program)
    {

        glUniformMatrix4fv( glGetUniformLocation( DepthMap_Program, "lightProj" ), 1, GL_FALSE, glm::value_ptr(lightProj) );
        glUniformMatrix4fv( glGetUniformLocation( DepthMap_Program, "lightView" ), 1, GL_FALSE, glm::value_ptr(lightView) );

        glUniformMatrix4fv( glGetUniformLocation( DepthMap_Program, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureID);
        glUniform1i(glGetUniformLocation(DepthMap_Program, "tex0"), 0);

        glBindVertexArray( vArray );
        glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );
        glBindVertexArray( 0 );

    }

    void DrawOnScreen(GLuint DepthMap_Texture){

        glm::mat3 matNormal = glm::transpose(glm::inverse(glm::mat3(matModel)));

        glUniformMatrix4fv( glGetUniformLocation( idProg, "matProj" ), 1, GL_FALSE, glm::value_ptr(matProj) );
        glUniformMatrix4fv( glGetUniformLocation( idProg, "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );
        glUniformMatrix4fv( glGetUniformLocation( idProg, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );
        glUniformMatrix3fv( glGetUniformLocation( idProg, "matNormal" ), 1, GL_FALSE, &(matNormal[0][0]) );

        glUniformMatrix4fv( glGetUniformLocation( idProg, "lightProj" ), 1, GL_FALSE, glm::value_ptr(lightProj) );
        glUniformMatrix4fv( glGetUniformLocation( idProg, "lightView" ), 1, GL_FALSE, glm::value_ptr(lightView) );


        // Swiatlo kierunkowe
        glUniform3fv( glGetUniformLocation( idProg, "Light_Direction" ), 1, glm::value_ptr(Light_Direction) );
        glUniform3fv( glGetUniformLocation( idProg, "Camera_Position" ), 1, glm::value_ptr(Camera_Position) );


        // Shadow map textura na slot 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DepthMap_Texture);
        glUniform1i(glGetUniformLocation( idProg, "shadowMap" ), 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureID);
        glUniform1i(glGetUniformLocation(idProg, "tex0"), 0);


        // Scena
        glBindVertexArray( vArray );
            glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );
        glBindVertexArray( 0 );

    }

};


#define __ALTITUDE_ERROR	99999

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
class CGround: public OpenGLObject {
public:

	int Number_of_triangles = 0;
	std::vector<CTriangle> triangles;

	// stworzenie struktury z listy trojkatow
	// na przyklad przy okazji ladowania z pliku OBJ
	void CreateFromOBJ()
	{
		glm::vec3 p1,p2,p3;

		std::vector<glm::vec3>::iterator it = OBJ_vertices.begin();
		while (it != OBJ_vertices.end())
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
	float getAltitute(glm::vec2 point, float currentHeight)
	{
		// obliczenie listy trojkatow nad ktorymi jestesmy
		CTriangle *collTriangle = NULL;
		CTriangle *temp = NULL;
        int ilosc_trojkatow = 0;
        float wys = 0.0;
        float prev = -99999.0;
        //wysokosc na jaka jest w stanie sie "wspiac"
        float epsilon = 0.5;

		for (auto it = triangles.begin(); it != triangles.end(); it++)
		{
			if ((*it).isInside(point))
			{
			    temp = &(*it);
			    wys = temp->altitude(point);
			    if ((wys - currentHeight) < epsilon) {
                    if (wys > prev){
                        prev = wys;
                        collTriangle = &(*it);
                    }
                    ilosc_trojkatow++;
			    }
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
	//zostawione aby drzewa mogly rysowac sie gdziekolwiek - i tak nie beda sie poruszaly
	float getAltitute(glm::vec2 point)
	{
		// obliczenie listy trojkatow nad ktorymi jestesmy
		CTriangle *collTriangle = NULL;
		CTriangle *temp = NULL;
        int ilosc_trojkatow = 0;
        float wys = 0.0;
        float prev = -99999.0;

		for (auto it = triangles.begin(); it != triangles.end(); it++)
		{
			if ((*it).isInside(point))
			{
			    temp = &(*it);
			    wys = temp->altitude(point);
                if (wys > prev){
                    prev = wys;
                    collTriangle = &(*it);
                }
                ilosc_trojkatow++;
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


class OpenGLObjectInstanced: public OpenGLObject {
public:

    int liczba_instancji = 0;
    //glm::mat4x4* matModelInst;
    std::vector<glm::mat4> matModelInst;
    float Fly;
    glm::vec3 *PositionInst;

    glm::vec3 Position;
    CGround *Ground = NULL;
    float radius = 0.5f;


    void Init(CGround *_ground)
    {
        Ground = _ground;
    }

    void ChangeInstanceNumber(int number){
        liczba_instancji = number;
        //matModelInst = new glm::mat4 [liczba_instancji];
        matModelInst.resize(number);
        PositionInst = new glm::vec3 [liczba_instancji];
    }

    void RandomizeLocationOfInstances(int RES=1000, int SIZE=30, float y = 0.0){

        for (int i=0; i<liczba_instancji; ++i)
        {
            Position.x = (rand()%RES)/(RES/SIZE) - SIZE/2.0;
            Position.z = (rand()%RES)/(RES/SIZE) - SIZE/2.0;

            float newAlt = Ground->getAltitute(glm::vec2(Position.x, Position.z), Position.y);
            while(newAlt == 0.0){
                Position.x = (rand()%RES)/(RES/SIZE) - SIZE/2.0;
                Position.z = (rand()%RES)/(RES/SIZE) - SIZE/2.0;
                newAlt = Ground->getAltitute(glm::vec2(Position.x, Position.z));
            }

            Position.y = newAlt + y;
            PositionInst[i] = Position;
            matModelInst[i] = glm::translate(glm::mat4(1.0), PositionInst[i]);
            float scale = (0.2 + (rand()%80)/100.0);

            float angle = (rand()%314)/100.0;

            //matModelInst[i] = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z) );
            matModelInst[i] = glm::rotate(matModelInst[i], angle, glm::vec3(0.0f, 1.0f, 0.0f));
            matModelInst[i] = glm::scale(matModelInst[i], glm::vec3(scale, scale, scale));
        }
    }
    void InitAtZero(){
        matModelInst[0] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
    }

    void Load(char *filename)
	{

		if (!loadOBJ(filename, OBJ_vertices, OBJ_uvs, OBJ_normals))
		{
			printf("Not loaded!\n");
			exit(1);
		}

		// Liczba wierzcholkow potrzebna do wyrenderowania obiektu
		Size = OBJ_vertices.size();

		// Vertex arrays
		glGenVertexArrays( 1, &this->vArray );
		glBindVertexArray( this->vArray );

		// Wspolrzedne wierzchokow
		GLuint vBuffer_coord;
		glGenBuffers( 1, &vBuffer_coord );
		glBindBuffer( GL_ARRAY_BUFFER, vBuffer_coord );
		glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 0 );

		// Wektory normalne
		GLuint vBuffer_normal;
		glGenBuffers( 1, &vBuffer_normal );
		glBindBuffer( GL_ARRAY_BUFFER, vBuffer_normal );
		glBufferData( GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 1 );

        //uv
		GLuint vBuffer_uv;
		glGenBuffers( 1, &vBuffer_uv );
        glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv );
        glBufferData( GL_ARRAY_BUFFER, OBJ_uvs.size() * sizeof(glm::vec2), &OBJ_uvs[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 2 );

        GLuint vInstances;
        glGenBuffers(1, &vInstances);
        glBindBuffer(GL_ARRAY_BUFFER, vInstances);
        glBufferData(GL_ARRAY_BUFFER, liczba_instancji * sizeof(glm::mat4), &matModelInst[0], GL_STATIC_DRAW);

        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);


        __CHECK_FOR_ERRORS
		glBindVertexArray( 0 );

	}
	void SetAnimationVar(float var){
        Fly = var;
	}

    void DrawOnScreen(GLuint DepthMap_Texture){
        //glUseProgram( idProg );


            glUniformMatrix4fv( glGetUniformLocation( idProg, "matProj" ), 1, GL_FALSE, glm::value_ptr(matProj) );
            glUniformMatrix4fv( glGetUniformLocation( idProg, "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );

            glUniformMatrix4fv( glGetUniformLocation( idProg, "lightProj" ), 1, GL_FALSE, glm::value_ptr(lightProj) );
            glUniformMatrix4fv( glGetUniformLocation( idProg, "lightView" ), 1, GL_FALSE, glm::value_ptr(lightView) );


            // Swiatlo kierunkowe
            glUniform3fv( glGetUniformLocation( idProg, "Light_Direction" ), 1, glm::value_ptr(Light_Direction) );
            glUniform3fv( glGetUniformLocation( idProg, "Camera_Position" ), 1, glm::value_ptr(Camera_Position) );


            // Shadow map textura na slot 1
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, DepthMap_Texture);
            glUniform1i(glGetUniformLocation( idProg, "shadowMap" ), 1);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, TextureID);
            glUniform1i(glGetUniformLocation(idProg, "tex0"), 0);

            glUniform1f(glGetUniformLocation(idProg, "Fly"), Fly);
            // Scena
            glBindVertexArray( vArray );
                glDrawArraysInstanced( GL_TRIANGLES, 0, OBJ_vertices.size(), liczba_instancji);
            glBindVertexArray( 0 );


        // WYLACZAMY program
        //glUseProgram(0);
    }

    void DrawToShadowMap(GLuint DepthMap_Program)
    {
        for (int i = 0; i < liczba_instancji; i++){

            glUniformMatrix4fv( glGetUniformLocation( DepthMap_Program, "lightProj" ), 1, GL_FALSE, glm::value_ptr(lightProj) );
            glUniformMatrix4fv( glGetUniformLocation( DepthMap_Program, "lightView" ), 1, GL_FALSE, glm::value_ptr(lightView) );

            glUniformMatrix4fv( glGetUniformLocation( DepthMap_Program, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModelInst[i]) );

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, TextureID);
            glUniform1i(glGetUniformLocation(DepthMap_Program, "tex0"), 0);

            glBindVertexArray( vArray );
            glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );
            glBindVertexArray( 0 );
        }

    }
    void RemoveInstance(int instance){
        printf("Removing instance: %d, lenght of vector = %d\n", instance, matModelInst.size());
        matModelInst.erase(matModelInst.begin() + instance);
        liczba_instancji -= 1;
        printf("lenght of vector = %d", matModelInst.size());
    }
    void MoveInstance(int instance, float x = 0.0, float y = 0.0, float z = 0.0){
        matModelInst[instance] = glm::translate(matModelInst[instance], glm::vec3(x, y, z));
    }
};

class CSceneObject: public OpenGLObject {
public:

    glm::vec3 Position;
    CGround *Ground = NULL;
    float radius = 1.0f;


    void Init(CGround *_ground)
    {
        Ground = _ground;
    }

    void UpdateLatitute()
	{
		float newAlt = Ground->getAltitute(glm::vec2(Position.x, Position.z), Position.y);

		Position.y = newAlt;
		matModel = glm::translate(glm::mat4(1.0), Position);
	}

	void MoveXZ(float _x, float _z)
    {
        Position += glm::vec3(_x, 0.0, _z);

        // Aktualizacja polozenia na Y
        UpdateLatitute();

    }
    void MoveXZ(float _x, float _z, const CSceneObject &_other)
    {
        glm::vec3 OldPosition = Position;

        Position += glm::vec3(_x, 0.0, _z);

        if(this->isCollision(_other)){
            Position = OldPosition;
        }

        UpdateLatitute();
    }

	// NOWE: sprawdzanie kolizji z innym obiektem
    bool isCollision(const CSceneObject &_other)
    {
        float distance = glm::distance(this->Position, _other.Position);
        if (distance < this->radius + _other.radius)
        {
            return true;
        }
        return false;
    }

    std::tuple<int, int> MoveXZ(float _x, float _z, std::vector<OpenGLObjectInstanced> _other, std::vector<int> type){

        glm::vec3 OldPosition = Position;

        for (int i = 0; i < _other.size(); i++){
            Position += glm::vec3(_x, 0.0, _z);
            auto [instance, isCollision] = this->isCollisionInstanced(_other[i]);
            if(isCollision && type[i] == 0){
                Position = OldPosition;
                return {instance, i};
            }
            else if (isCollision && type[i] == 1){
                _other[i].RemoveInstance(instance);
                _other[i].liczba_instancji = _other[i].liczba_instancji - 1;
                punkty += 1;
            }

        }

        UpdateLatitute();
        return {9999, _other.size()+2};
    }

    std::tuple<int, bool> isCollisionInstanced(OpenGLObjectInstanced _other)
    {
        for (int i = 0; i < _other.liczba_instancji; i++){
            float distance = glm::distance(this->Position, _other.PositionInst[i]);
            if (distance < this->radius + _other.radius)
            {
                //_other.RemoveInstance(i);
                //_other.MoveInstance(i, 0.0, 5.0);
                return {i, true};
            }
        }
        return {_other.liczba_instancji+3, false};
    }
};


#endif
