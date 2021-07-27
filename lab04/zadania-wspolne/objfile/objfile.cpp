#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_stuff.hpp"
#include "obj_loader.hpp"


glm::mat4 matProj;
glm::mat4 matView;
glm::mat4 matProjView;
glm::mat4 matView2;


// Identyfikatory obiektow

GLuint idProgram;	// programu
GLuint idVAO;		// tablic wierzcholkow
GLuint idVBO_coord;	// bufora na wspolrzedne
GLuint idVBO_color; // bufora na kolory


GLuint idVAO2;		// tablic wierzcholkow
GLuint idVBO_coord2;	// bufora na wspolrzedne

// -----------------------------------
// NOWE: wektory na dane z pliku OBJ
// -----------------------------------
std::vector<glm::vec3> OBJ_vertices;
std::vector<glm::vec2> OBJ_uvs;
std::vector<glm::vec3> OBJ_normals;

std::vector<glm::vec3> OBJ_vertices2;
std::vector<glm::vec2> OBJ_uvs2;
std::vector<glm::vec3> OBJ_normals2;
GLfloat zmienna1 = 0.0;

// ---------------------------------------
void DisplayScene()
{

	// Macierz widoku
	matView = glm::mat4x4( 1.0 );
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    matView = glm::rotate( matView, zmienna1, glm::vec3( 0.0f, 1.0f, 0.0f) );
	// Obliczanie macierzy rzutowania
	matProjView = matProj * matView;

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Wlaczenie VAO i programu
	glBindVertexArray( idVAO );
	glUseProgram( idProgram );

		// Przekazanie macierzy rzutowania
		glUniformMatrix4fv( glGetUniformLocation(idProgram, "matProjView"), 1, GL_FALSE,  glm::value_ptr(matProjView));

		// Generowanie obiektow na ekranie
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );


	// Wylaczanie
	glUseProgram( 0 );
	glBindVertexArray( 0 );

    zmienna1 += 0.01;
	matView2 = glm::mat4x4( 1.0 );
	matView2 = glm::translate( matView2, glm::vec3( _scene_translate_x, _scene_translate_y+2, _scene_translate_z) );
	matView2 = glm::rotate( matView2, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView2 = glm::rotate( matView2, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    matView2 = glm::rotate( matView2, zmienna1, glm::vec3( 1.0f, 0.0f, 0.0f) );

	// Obliczanie macierzy rzutowania
	matProjView = matProj * matView2;

	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Wlaczenie VAO i programu
	glBindVertexArray( idVAO2 );
	glUseProgram( idProgram );

		// Przekazanie macierzy rzutowania
		glUniformMatrix4fv( glGetUniformLocation(idProgram, "matProjView"), 1, GL_FALSE,  glm::value_ptr(matProjView));

		// Generowanie obiektow na ekranie
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices2.size() );


	// Wylaczanie
	glUseProgram( 0 );
	glBindVertexArray( 0 );










	glutSwapBuffers();

}

void Animation ( int keyframe )
{
	glutPostRedisplay ();
	glutTimerFunc (0 , Animation ,0);
}

// ---------------------------------------
void Initialize()
{

	// -----------------------------------
	// NOWE: wczytanie pliku obj
	// -----------------------------------

	if (!loadOBJ("scene.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
	{
		printf("Not loaded!\n");
		exit(1);
	}

	if (!loadOBJ("monkey.obj", OBJ_vertices2, OBJ_uvs2, OBJ_normals2))
	{
		printf("Not loaded!\n");
		exit(1);
	}

	// ustawienie poczatkowej translacji na z
	_scene_translate_z = -10.0f;

	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	glEnable(GL_DEPTH_TEST);

	// 1. Program i shadery
	idProgram = glCreateProgram();

	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram( idProgram );


	// 2. Vertex arrays
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers( 1, &idVBO_coord );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );


    // 2. Vertex arrays
	glGenVertexArrays( 1, &idVAO2 );
	glBindVertexArray( idVAO2 );

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers( 1, &idVBO_coord2 );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord2 );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices2.size() * sizeof(glm::vec3), &OBJ_vertices2[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	/*
	glGenBuffers( 1, &idVBO_coord );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

    */

	glBindVertexArray( 0 );

}

// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );

	matProj = glm::ortho(-1.0, 1.0, -1.0, 1.0);
	matProj = glm::perspective(glm::radians(80.0f), width/(float)height, 0.1f, 20.0f);

}




// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 500, 500 );
	glutCreateWindow( "Programownie grafiki w OpenGL" );

	// -----------------------------
	// NOWE: Handlery
	// -----------------------------

	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutMouseWheelFunc( MouseWheel );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( SpecialKeys );


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

    glutTimerFunc (100 , Animation , 0);
	glutMainLoop();


	// Cleaning
	glDeleteProgram( idProgram );
	glDeleteVertexArrays( 1, &idVBO_coord );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}
