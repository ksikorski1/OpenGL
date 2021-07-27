// ------------------------------------------------
// Szablon aplikacji w OpenGL korzystajacej
// z GLFW zamiast freeGLUT
//
// Kompilacja pod windowsem
// gcc plik.cpp -glfw3 -lgdi32
//
// Wymagana jest bibliteka glad
// https://glad.dav1d.de/ 
//
// Dolaczony plik glad.hpp (oryginalnie glad.c)
// jest stworzony pod Opengl 4.5 i nalezy go 
// podmienic plikiem glad.c pobranym z powyzszej 
// strony.
// ------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glad.hpp"

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
		return 0;
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
	"uniform float param;       "
	"void main()				"
	"{							"
	"	outColor = vec4( param, 1.f, 0.f, 1.f ); "
	"}";


	glShaderSource( shader, 1, &code, NULL );
	glCompileShader( shader );

	GLint status;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
	if( status != GL_TRUE )
	{
		printf("Blad kompilacji fragment shadera!\n");
		return;
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



// ---------------------------------------------------
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

}



// ---------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport( 0, 0, width, height );
}

// ---------------------------------------------------
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);

}

// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	GLFWwindow* window;
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

	window = glfwCreateWindow(800, 600, "Programowanie grafiki w OpenGL (fakultet)", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetKeyCallback(window, key_callback);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);
	//glfwSetCursorPosCallback(window, cursor_position_callback);
	//glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glfwSwapInterval(1);

	Initialize();

	float param = 0.0;

	while (!glfwWindowShouldClose(window))
	{


		param += 0.05;
		if (param > 1.0) param = 0.0;


			// 1. czyszczenie bufora ramki koloru
			glClear( GL_COLOR_BUFFER_BIT );

			// 2. wlaczenie obiektu tablic wierzcholkow
			glBindVertexArray( idVAO );

			// 3. aktywowanie programu
			glUseProgram( idProgram );

			glUniform1f(glGetUniformLocation(idProgram, "param"), param);

			// 4. uruchomienie aktualnego programu na aktualnej tablicy wierzcholkow
			glDrawArrays( GL_TRIANGLES, 0, 1*3 );

			// 5. wylaczenie programu
			glUseProgram( 0 );

			// 6. wylaczenie obiektu tablic wierzcholkow
			glBindVertexArray( 0 );


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);

	return 0;
}
