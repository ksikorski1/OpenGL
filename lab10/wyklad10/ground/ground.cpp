#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "texture_loader.hpp"
#include "shader_stuff.h"


enum {
	SCENE = 0,
	SPHERE,
	NUMBER_OF_BUFFERS,
};

enum {
	PROGRAM_SCENE = 0,
	NUMBER_OF_PROGRAMS,
};



// ---------------------------------------
GLuint program[NUMBER_OF_PROGRAMS];
GLuint vBuffer_pos[NUMBER_OF_BUFFERS];
GLuint vBuffer_uv[NUMBER_OF_BUFFERS];
GLuint vBuffer_normal[NUMBER_OF_BUFFERS];
GLuint vArray[NUMBER_OF_BUFFERS];


// ---------------------------------------
glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matModel;


// ---------------------------------------
std::vector<glm::vec3> OBJ_vertices[2];
std::vector<glm::vec2> OBJ_uvs[2];
std::vector<glm::vec3> OBJ_normals[2];

GLuint TextureID[NUMBER_OF_BUFFERS];



// ---------------------------------------
// NOWE: obiekt klasy CGround i CSceneObject
// ---------------------------------------
#include "ground.h"

#include "object.h"

// ---------------------------------------

CGround ziemia;
CSceneObject postac;


// ---------------------------------------
void DisplayScene()
{

	matView = glm::mat4x4( 1.0 );
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );

    // Macierz modelu
    matModel =  glm::mat4x4( 1.0 );


	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// AKTYWUJEMY program
	glUseProgram( program[PROGRAM_SCENE] );


		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matProj" ), 1, GL_FALSE, glm::value_ptr(matProj) );
		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );
		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );


		// AKTYWUJEMY tekstury
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(program[PROGRAM_SCENE], "tex0"), 0);


		// Scena
		glBindVertexArray( vArray[SCENE] );
			glBindTexture(GL_TEXTURE_2D, TextureID[SCENE]);
			glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[SCENE].size() );
		glBindVertexArray( 0 );


        glBindTexture(GL_TEXTURE_2D, TextureID[SPHERE]);
        postac.Draw();








    // WYLACZAMY program
    glUseProgram(0);



	glutSwapBuffers();
}



// ---------------------------------------------------
void Initialize()
{
	_scene_translate_z = -5;
	_scene_translate_y = -2;

	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

    // ---------------------------------------
	// Ladowanie pliku OBJ
	if (!loadOBJ("scene-levels.obj", OBJ_vertices[SCENE], OBJ_uvs[SCENE], OBJ_normals[SCENE]))
	{
		printf("Not loaded!\n");
		exit(1);
	}


	// ---------------------------------------
	// Tworzenie tekstury
	int tex_width;
	int tex_height;
	unsigned char *tex_data;
	loadBMP_custom("chess.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID[SCENE]);
	glBindTexture(GL_TEXTURE_2D, TextureID[SCENE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);



	// Tworzenie potoku OpenGL
	program[PROGRAM_SCENE] = glCreateProgram();
	glAttachShader( program[PROGRAM_SCENE], LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program[PROGRAM_SCENE], LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( program[PROGRAM_SCENE] );


	// SCENA
	glGenVertexArrays( 1, &vArray[SCENE] );
	glBindVertexArray( vArray[SCENE] );

	glGenBuffers( 1, &vBuffer_pos[SCENE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[SCENE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[SCENE].size() * sizeof(glm::vec3), &(OBJ_vertices[SCENE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[SCENE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[SCENE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[SCENE].size() * sizeof(glm::vec2), &(OBJ_uvs[SCENE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );


    // Inne ustawienia openGL i sceny
	glEnable( GL_DEPTH_TEST );




    // SPHERE
    if (!loadOBJ("sphere.obj", OBJ_vertices[SPHERE], OBJ_uvs[SPHERE], OBJ_normals[SPHERE]))
	{
		printf("Not loaded!\n");
		exit(1);
	}
	// Vertex arrays
	glGenVertexArrays( 1, &vArray[SPHERE] );
	glBindVertexArray( vArray[SPHERE] );

	glGenBuffers( 1, &vBuffer_pos[SPHERE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[SPHERE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[SPHERE].size() * sizeof(glm::vec3), &(OBJ_vertices[SPHERE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[SPHERE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[SPHERE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[SPHERE].size() * sizeof(glm::vec2), &(OBJ_uvs[SPHERE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );

    loadBMP_custom("bubbles.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID[SPHERE]);
	glBindTexture(GL_TEXTURE_2D, TextureID[SPHERE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


    ziemia.CreateFromOBJ(OBJ_vertices[SCENE]);

    postac.Init(vArray[SPHERE], OBJ_vertices[SCENE].size(), &ziemia);
    postac.SetPosition(0,0,0);

}


// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}

// --------------------------------------------------------------
void Keyboard( unsigned char key, int x, int y )
{

float move_vec = 0.2;

    switch(key)
    {
		case 27:	// ESC key
			exit(0);
			break;

        case 'w':
            postac.MoveXZ(move_vec, 0);
            break;

        case 's':
            postac.MoveXZ(-move_vec, 0);
            break;

        case 'd':
            postac.MoveXZ(0, move_vec);
            break;

        case 'a':
            postac.MoveXZ(0, -move_vec);
            break;

    }

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
	glutInitWindowSize( 800, 600);
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
