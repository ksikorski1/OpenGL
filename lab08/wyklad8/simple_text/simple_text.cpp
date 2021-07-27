#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "shader_stuff.h"


// ---------------------------------------
// NOWE: Tekst na ekranie
// ---------------------------------------
#include "text.h"



// Identyfikatory obiektow
GLuint program;
GLuint vArray;
glm::mat4x4 Matrix_proj;
std::vector<glm::vec3> OBJ_vertices;


int frame = 0;

// ---------------------------------------
void DisplayScene()
{

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	glm::mat4x4 Matrix_mv = glm::translate( glm::mat4(1.0), glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	Matrix_mv = glm::rotate( Matrix_mv, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	Matrix_mv = glm::rotate( Matrix_mv, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	glm::mat4x4 Matrix_proj_mv = Matrix_proj * Matrix_mv;


    // Generowanie obiektow na ekranie (standardowo)
	glUseProgram( program );

		glUniformMatrix4fv( glGetUniformLocation( program, "Matrix_proj_mv" ), 1, GL_FALSE, glm::value_ptr(Matrix_proj_mv) );

		glBindVertexArray( vArray );
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );
		glBindVertexArray( 0 );
    glUseProgram( 0 );


	// ---------------------------------------
	// NOWE: Tekst na ekranie
	// ---------------------------------------
	// Generowanie tekstu na ekranie
    DrawText8x16( 3, 3, "Staly tekst" );

    char txt[255];
    sprintf(txt, "Klatka %d!", frame++);
    DrawText8x16( 3, 21, txt, glm::vec4(1.0, 1.0, 0.0, 1.0) );
    DrawText8x16( 300, 3, "ESC - Wyjscie" );


	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
	Matrix_proj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}

// ---------------------------------------------------
void Initialize()
{

	// ---------------------------------------
	// NOWE: Tekst na ekranie
	// ---------------------------------------
	// Inicjalizacja funkcji rysujacych tekst na ekranie
	InitDrawText();



	// Dalej standardowo
	std::vector<glm::vec2> OBJ_uvs;
	std::vector<glm::vec3> OBJ_normals;
	if (!loadOBJ("scene.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
	{
		printf("Not loaded!\n");
		exit(1);
	}


	_scene_translate_z = -10;
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
	glEnable( GL_DEPTH_TEST );

	program = glCreateProgram();
	glAttachShader( program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( program );

	glGenVertexArrays( 1, &vArray );
	glBindVertexArray( vArray );
	GLuint vBuffer_coord;
	glGenBuffers( 1, &vBuffer_coord );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_coord );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

}


// ---------------------------------------------------
void Animation(int frame)
{
    glutTimerFunc(5, Animation, 0);
    glutPostRedisplay();
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

    glutTimerFunc(5, Animation, 0);
	glutMainLoop();

	// Cleaning();
	glDeleteProgram( program );
	glDeleteVertexArrays( 1, &vArray );

    DeleteDrawText();
	return 0;
}
