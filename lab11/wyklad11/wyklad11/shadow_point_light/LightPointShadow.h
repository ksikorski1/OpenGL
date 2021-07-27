// Reference: https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows

#ifndef __SHADOW_POINT_LIGHT
#define __SHADOW_POINT_LIGHT

// ---------------------------------------
class CLightPointShadow
{

public:

	// Polozenie swiatla
	glm::vec3 Position;

	// Identyfikatory
	GLuint idTexture;		// tekstura
	GLuint idProgram;		// program generujacy mape cieni
	GLuint idFrameBuffer;	// ramka buforu (rendering pozaekranowy)

	// Rozmiar mapy cieni
	int Shadow_Width = 1024;
	int Shadow_Height = 1024;

	// Bryla obcinania (frustum)
	float frustumNear = 1.0f;
	float frustumFar = 40.0f;
	glm::mat4 matProj;
	glm::mat4 matPVArray[6];


	// ----------------------------------------------------------
	// Inicjalizacja
	void Init(glm::vec3 _pos);

	// Etap 1: Generowanie mapy cieni
	// pomiedzy tymi funkcjami rysujemy to co ma rzucac cien
	void GenBegin(void);
	void GenEnd(void);

	// Aktualizacja polozenia oswietlenia i macierzy rzutowania
	void UpdatePV();


	// Etap 2: Podczas renderingu na ekran
	// Wysylanie do programu _prog wszystkich potrzebnych uniformow
	// Ustawianie tekstury mapy cieni na slot _texSlot
	void SendUniform(GLuint _prog, GLuint _texSlot, glm::vec3 _camPos);

};

// -------------------------------------------------------
// Wyslij wszystkie potrzebne uniformy i teksture do programu
void CLightPointShadow::SendUniform(GLuint _prog, GLuint _texSlot, glm::vec3 _camPos)
{

	// Shadow map textura na slot _texSlot
	glActiveTexture(GL_TEXTURE0 + _texSlot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, idTexture);
	glUniform1i(glGetUniformLocation( _prog, "ShadowMapCube" ), _texSlot);

	// Parametry oswietlenia
	glUniform3fv( glGetUniformLocation( _prog, "LightPosition" ), 1, glm::value_ptr(Position) );
	glUniform1f( glGetUniformLocation( _prog, "FarPlane" ), frustumFar );

	// Inne : pozycja kamery
	glUniform3fv( glGetUniformLocation( _prog, "CameraPosition" ), 1, glm::value_ptr(_camPos) );


}

// -------------------------------------------------------
void CLightPointShadow::GenBegin(void)
{

	glViewport(0, 0, Shadow_Width, Shadow_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	// AKTYWUJEMY program
	glUseProgram( idProgram );

	// Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}


// -------------------------------------------------------
void CLightPointShadow::GenEnd(void)
{

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	// WYLACZAMY program
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// -------------------------------------------------------
// Aktualizacja tablicy 6 macierzy rzutowania
void CLightPointShadow::UpdatePV()
{
	// Obliczanie nowych macierzy rzutowania
	matPVArray[0] = matProj * glm::lookAt(Position, Position + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0));
	matPVArray[1] = matProj * glm::lookAt(Position, Position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0));
	matPVArray[2] = matProj * glm::lookAt(Position, Position + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
	matPVArray[3] = matProj * glm::lookAt(Position, Position + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0));
	matPVArray[4] = matProj * glm::lookAt(Position, Position + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0));
	matPVArray[5] = matProj * glm::lookAt(Position, Position + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0));

	// Wyslanie do programu
	glUseProgram( idProgram );

	for (int i = 0; i < 6; ++i)
	{
		char name[100];
		sprintf(name, "matPVArray[%i]", i);
		glUniformMatrix4fv( glGetUniformLocation( idProgram, name ), 1, GL_FALSE, glm::value_ptr(matPVArray[i]));
	}

	glUniform3fv( glGetUniformLocation( idProgram, "LightPosition" ), 1, glm::value_ptr(Position) );
	glUniform1f( glGetUniformLocation( idProgram, "FarPlane" ), frustumFar );

}


// -------------------------------------------------------
void CLightPointShadow::Init(glm::vec3 _pos)
{
	// 1. Pozycja zrodla swiatla
	this->Position = _pos;

	// 2. Textura do depthMapy
	glGenTextures(1, &idTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, idTexture);

	for (int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, Shadow_Width, Shadow_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// 3. Framebuffer z buforem glebokosci
	glGenFramebuffers(1, &idFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, idTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 4. Program Tworzenie potoku OpenGL
	idProgram = glCreateProgram();
	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "shaders/LightPointShadow_Depth.vert"));
	glAttachShader( idProgram, LoadShader(GL_GEOMETRY_SHADER, "shaders/LightPointShadow_Depth.geom"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "shaders/LightPointShadow_Depth.frag"));
	LinkAndValidateProgram( idProgram );


	// 5. Ustawienia kamery cienia
	matProj = glm::perspective(glm::radians(90.0f), Shadow_Width/(float)Shadow_Height, frustumNear, frustumFar);
	// aktualizacja macierzu rzutowania
	UpdatePV();

}





#endif
