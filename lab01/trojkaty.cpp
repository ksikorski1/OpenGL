#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>


// Identyfikatory obiektow
GLuint idProgram;	// programu
GLuint idVBO;		// bufora wierzcholkow
GLuint idVAO;		// tablic wierzcholkow

// Wspolrzedne wierzcholkow
GLfloat triangles[1*3*2] =
{
	-1.f, -1.f,
	1.f, -1.f,
	0.f,  1.f
};

// ---------------------------------------
GLuint CreateVertexShader( void )
{
	GLuint shader = glCreateShader( GL_VERTEX_SHADER );

	const GLchar * code =
	"#version 330 \n			"
	"in vec4 inPosition;	"
	"void main() 				"
	"{ 							"
	"	gl_Position = inPosition; "
	"}							";

	glShaderSource( shader, 1, &code, NULL );
	glCompileShader( shader );

	GLint status;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
	if( status != GL_TRUE )
	{
		printf("Blad kompilacji vertex shadera!\n");
		exit(1);
	}
	glAttachShader( idProgram, shader );
}

// ---------------------------------------
void CreateFragmentShader( void )
{
	GLuint shader = glCreateShader( GL_FRAGMENT_SHADER );

	const GLchar *code =
	"#version 150 \n			"
	"out vec4 outColor;			"
	"void main()				"
	"{							"
	"	outColor = vec4( 0.f, 1.f, 0.f, 1.f ); "
	"}";


	glShaderSource( shader, 1, &code, NULL );
	glCompileShader( shader );

	GLint status;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
	if( status != GL_TRUE )
	{
		printf("Blad kompilacji fragment shadera!\n");
		exit(1);
	}
	glAttachShader( idProgram, shader );
}


// ---------------------------------------
void Initialize()
{
	// ustawianie koloru ktorym bedzie czyszczony bufor ramki
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

	// 1. Program

	// Stworzenie programu i shaderow
	idProgram = glCreateProgram();
	CreateVertexShader();
	CreateFragmentShader();

	// Walidacja programu
	glLinkProgram( idProgram );
	glValidateProgram( idProgram );

	// 2. Dane

	// Stworzenie identyfikatora obiektu oraz obiektu tablic wierzcholkow
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );

	// Generowanie identyfikatora obiektu oraz obiektu bufora
	glGenBuffers( 1, &idVBO );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO );

	// Zaladowanie danych do obiektu bufora wierzcholkow
	glBufferData( GL_ARRAY_BUFFER, sizeof(float)*6, triangles, GL_STATIC_DRAW );

	// Ustawienie indeksu atrybutu wierzcholka o nazwie "inPosition"
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	// wylaczenie obiektu tablic wierzcholkow
	glBindVertexArray( 0 );

}


// ---------------------------------------
void DisplayScene()
{
	// 1. czyszczenie bufora ramki koloru
	glClear( GL_COLOR_BUFFER_BIT );

	// 2. wlaczenie obiektu tablic wierzcholkow
	glBindVertexArray( idVAO );

	// 3. aktywowanie programu
	glUseProgram( idProgram );

	// 4. uruchomienie aktualnego programu na aktualnej tablicy wierzcholkow
	glDrawArrays( GL_TRIANGLES, 0, 1*3 );

	// 5. wylaczenie programu
	glUseProgram( 0 );

	// 6. wylaczenie obiektu tablic wierzcholkow
	glBindVertexArray( 0 );

	// 7. wyswietlanie na ekranie (tryb podwojnego buforowania)
	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
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
	glutCreateWindow( "Hello World!" );

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
	glDeleteProgram( idProgram );
	glDeleteBuffers( 1, &idVBO );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}
