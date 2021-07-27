// References:
// - https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
// - https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
// - Janusz Ganczarski, OpenGL. Podstawy programowania grafiki 3D, Helion.

#define __CHECK_FOR_ERRORS 	{GLenum errCode; if ((errCode = glGetError()) != GL_NO_ERROR) printf("Error (%d): %s in file %s at line %d !\n", errCode, gluErrorString(errCode), __FILE__,  __LINE__);}

#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "shader_stuff.h"


enum {
	SCENE = 0,
	NUMBER_OF_BUFFERS,
};

enum {
	PROGRAM_SCENE = 0,
	NUMBER_OF_PROGRAMS,
};


// ---------------------------------------
GLuint program[NUMBER_OF_PROGRAMS];
GLuint vBuffer_pos[NUMBER_OF_BUFFERS];
GLuint vBuffer_normals[NUMBER_OF_BUFFERS];
GLuint vBuffer_uvs[NUMBER_OF_BUFFERS];
GLuint vArray[NUMBER_OF_BUFFERS];


// ---------------------------------------
glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matModel;


int Window_Width;
int Window_Height;

// ---------------------------------------
std::vector<glm::vec3> OBJ_vertices[NUMBER_OF_BUFFERS];
std::vector<glm::vec3> OBJ_normals[NUMBER_OF_BUFFERS];
std::vector<glm::vec2> OBJ_uvs[NUMBER_OF_BUFFERS];


// -------------------------------------------------------
// NOWE: Obiekt oswietlenia punktowego
#include "LightPointShadow.h"
CLightPointShadow myLightPoint;
// -------------------------------------------------------



// -------------------------------------------------------
void DrawOnScreen()
{
	// Macierz widoku
	matView = glm::mat4x4( 1.0 );
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );

	glm::vec3 Camera_Position = ExtractCameraPos(matView);

    matModel =  glm::mat4x4( 1.0 );

	// AKTYWUJEMY program
	glUseProgram( program[PROGRAM_SCENE] );


		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matProj" ), 1, GL_FALSE, glm::value_ptr(matProj) );
		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );
		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );


		// -------------------------------------------------------
		// NOWE: Wyslij wszystko co potrzebuje ShadowPoint
		// -------------------------------------------------------
		myLightPoint.SendUniform(program[PROGRAM_SCENE], 1, Camera_Position);


		// Scena
		glBindVertexArray( vArray[SCENE] );
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[SCENE].size() );
		glBindVertexArray( 0 );


    // WYLACZAMY program
    glUseProgram(0);
}

// -------------------------------------------------------
void DisplayScene()
{
	__CHECK_FOR_ERRORS;

	// -------------------------------------------------------
	// NOWE: Generowanie mapy cieni
	// -------------------------------------------------------

	// Uruchomienie programu do generowania mapy cieni
	// Przeslanie wszystkich potrzebnych informacji do shaderow
	myLightPoint.GenBegin();


		// Renderujemy obiekt przy uzyciu aktualnego programu
		glBindVertexArray( vArray[SCENE] );
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[SCENE].size() );
		glBindVertexArray( 0 );

	// Zakonczenie procesu generowania cieni
	myLightPoint.GenEnd();


	// -------------------------------------------------------
	// 2. Renderowanie z pozycji kamery na ekran (Rendering finalny)

	// -------------------------------------------------------
	glViewport(0, 0, Window_Width, Window_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawOnScreen();


	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
	Window_Width = width;
	Window_Height = height;

	glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 150.f );
}

// ---------------------------------------------------
void Initialize()
{
	_scene_translate_z = -25;
	_scene_translate_y = 0;

	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

	// Ladowanie pliku OBJ
	if (!loadOBJ("scene.obj", OBJ_vertices[SCENE], OBJ_uvs[SCENE], OBJ_normals[SCENE]))
	{
		printf("Not loaded!\n");
		exit(1);
	}

	// Tworzenie potoku OpenGL
	program[PROGRAM_SCENE] = glCreateProgram();
	glAttachShader( program[PROGRAM_SCENE], LoadShader(GL_VERTEX_SHADER, "shaders/LightPointShadow_Main.vert"));
	glAttachShader( program[PROGRAM_SCENE], LoadShader(GL_FRAGMENT_SHADER, "shaders/LightPointShadow_Main.frag"));
	LinkAndValidateProgram( program[PROGRAM_SCENE] );

	// SCENA
	glGenVertexArrays( 1, &vArray[SCENE] );
	glBindVertexArray( vArray[SCENE] );

	glGenBuffers( 1, &vBuffer_pos[SCENE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[SCENE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[SCENE].size() * sizeof(glm::vec3), &(OBJ_vertices[SCENE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_normals[SCENE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_normals[SCENE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_normals[SCENE].size() * sizeof(glm::vec3), &(OBJ_normals[SCENE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );

    // Inne ustawienia openGL i sceny
	glEnable( GL_DEPTH_TEST );



	// -------------------------------------------------------
	// NOWE: Inicjalizacja oswietlenia punktowego w ustalonym polozeniu
	// -------------------------------------------------------
	myLightPoint.Init(glm::vec3(0.0, 3.0, 0.0));

}



// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( Window_Width, Window_Height );
	glutCreateWindow( "OpenGL" );

	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( GLEW_OK != err )
	{
		printf("GLEW Error\n");
		exit(1);
	}

	// OpenGL
	if( !GLEW_VERSION_3_2 )
	{
		printf("Brak OpenGL 3.2!\n");
		exit(1);
	}


	Initialize();
	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutMouseWheelFunc( MouseWheel );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( SpecialKeys );

	glutMainLoop();


	return 0;
}
