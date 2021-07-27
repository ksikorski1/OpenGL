#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "shader_stuff.h"


// ---------------------------------------
// Identyfikatory obiektow

GLuint program;
GLuint vBuffer_coord;
GLuint vBuffer_color;
GLuint vBuffer_indices;
GLuint vArray;

// ---------------------------------------
// Wspolrzedne wierzchokow
GLfloat triangles_coord[2*6] =
{
	-0.5f, -1.0f,
	 0.5f, -1.0f,
	 1.0f,  0.0f,
	 0.5f,  1.0f,
	-0.5f,  1.0f,
	-1.0f,  0.0f
};

// ---------------------------------------
// Kolory wierzcholkow
GLfloat triangles_color[3*6] =
{
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 0.f, 1.f,
	1.f, 1.f, 0.f,
	0.f, 1.f, 1.f,
	1.f, 0.f, 1.f
};

// ---------------------------------------
// Indeksy wierzcholkow
GLuint triangles_indices[3*4] =
{
	0, 1, 2,
	0, 2, 3,
	0, 3, 4,
	0, 4, 5
};

// ---------------------------------------
void DisplayScene()
{

	glClear( GL_COLOR_BUFFER_BIT );

	// Wlaczenie VA i programu
	glBindVertexArray( vArray );
	glUseProgram( program );

	// Generowanie obiektow na ekranie

	// Bez tablicy wierzcholkow
	//glDrawArrays( GL_TRIANGLES, 0, 2*3 );

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Z tablica wierzcholkow
	glDrawElements( GL_TRIANGLES, 4 * 3, GL_UNSIGNED_INT, NULL );




	glUseProgram( 0 );
	glBindVertexArray( 0 );
	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
}


// ---------------------------------------
void Initialize()
{

	glClearColor( 0.6f, 0.6f, 0.6f, 1.0f );


	// 1. Program i shadery
	program = glCreateProgram();

	glAttachShader( program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram( program );



	// 2. Vertex arrays

	#define POSITION 0
	#define COLOR 1

	glGenVertexArrays( 1, &vArray );
	glBindVertexArray( vArray );

	// BUFOR: Wspolrzedne wierzcholkow

	glGenBuffers( 1, &vBuffer_coord );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_coord );
	glBufferData( GL_ARRAY_BUFFER, sizeof( triangles_coord ), triangles_coord, GL_STATIC_DRAW );

	glVertexAttribPointer( POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( POSITION );

	// BUFOR: Kolor wierzcholkow

	glGenBuffers( 1, &vBuffer_color );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_color  );
	glBufferData( GL_ARRAY_BUFFER, sizeof( triangles_color ), triangles_color, GL_STATIC_DRAW );

	glVertexAttribPointer( COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( COLOR );

	// BUFOR: Indeksy wierzcholkow

    glGenBuffers( 1, &vBuffer_indices );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vBuffer_indices );
    //lepiej sizeof (typ) * jego_ilosc
    //typ - np. GLfloat albo GLuint
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( triangles_indices ), triangles_indices, GL_STATIC_DRAW );


	glBindVertexArray( 0 );

}




// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 500, 500 );
	glutCreateWindow( "Tablice wierzcholkow" );
	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );


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


	glutMainLoop();

	// Cleaning
	glDeleteProgram( program );
	glDeleteBuffers( 1, &vBuffer_coord );
	glDeleteBuffers( 1, &vBuffer_color );
	glDeleteVertexArrays( 1, &vArray );

	return 0;
}
