// ---------------------------------------------------
// Textures: http://humus.name/index.php?page=Textures
// ---------------------------------------------------

#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "shader_stuff.h"


// ---------------------------------------
// Program/programy dla normalnych obiektow
// ---------------------------------------

GLuint program;
GLuint vArray;
GLuint TextureID;

std::vector<glm::vec3> OBJ_vertices;
std::vector<glm::vec2> OBJ_uvs;
std::vector<glm::vec3> OBJ_normals;


// ---------------------------------------
// Program dla SkyBox'a
// ---------------------------------------

GLuint SkyBox_Program;
GLuint SkyBox_VAO;
GLuint SkyBox_Texture;


// ---------------------------------------
// Scena, kamera i macierze
// ---------------------------------------

glm::mat4x4 Matrix_proj;
glm::mat4x4 Matrix_view;
glm::mat4x4 Matrix_proj_view;

glm::vec3 Camera_Position;



// ---------------------------------------
void DisplayScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	Matrix_view = glm::mat4x4( 1.0 );
	Matrix_view = glm::translate( Matrix_view, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	Matrix_view = glm::rotate( Matrix_view, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	Matrix_view = glm::rotate( Matrix_view, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	Matrix_proj_view = Matrix_proj * Matrix_view;
    Camera_Position = ExtractCameraPos(Matrix_view);


	// -------------------------------------------------
	// Program: glowny domyslny do renderowania obiektow
	// -------------------------------------------------
	glUseProgram(program);

		// Wyslanie macierzy rzutowania
		glUniformMatrix4fv( glGetUniformLocation( program, "Matrix_proj_view" ), 1, GL_FALSE, glm::value_ptr(Matrix_proj_view) );
		glUniform3fv( glGetUniformLocation( program, "Camera_Position" ), 1, &Camera_Position[0] );

		// Aktywacja tekstury
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		// Aktywacja tekstury CUBE_MAP
		glActiveTexture(GL_TEXTURE1);
		glBindTexture( GL_TEXTURE_CUBE_MAP, SkyBox_Texture );

		glUniform1i(glGetUniformLocation(program, "tex0"), 0);
		glUniform1i(glGetUniformLocation(SkyBox_Program, "skybox"), 1);

		// Renderowanie obiektu/obiektow
		glBindVertexArray( vArray );
			glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );
		glBindVertexArray( 0  );

	glUseProgram(0);


	// ----------------------------------
	// Program: SKY BOX
	// ----------------------------------
	glUseProgram(SkyBox_Program);


		// Przeskalowanie boxa i przeslanie macierzy rzutowania
		Matrix_proj_view = Matrix_proj * Matrix_view * glm::scale(glm::mat4(1), glm::vec3(40.0, 40.0, 40.0));

		glUniformMatrix4fv( glGetUniformLocation( SkyBox_Program, "Matrix_proj_view" ), 1, GL_FALSE, glm::value_ptr(Matrix_proj_view) );
		glUniform1i(glGetUniformLocation(SkyBox_Program, "tex0"), 0);
		glUniform3fv( glGetUniformLocation( SkyBox_Program, "Camera_Position" ), 1, &Camera_Position[0] );

		// Aktywacja tekstury CUBE_MAP
		glActiveTexture(GL_TEXTURE0);
		glBindTexture( GL_TEXTURE_CUBE_MAP, SkyBox_Texture );

		// Rendering boxa
		glBindVertexArray( SkyBox_VAO );
			glDrawElements( GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, NULL );
		glBindVertexArray( SkyBox_VAO );

	glUseProgram(0);



	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
	Matrix_proj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}


// ------------------------------------------
// NOWE: Tworzenie programu i obiektu skybox
// ------------------------------------------
void CreateSkyBox()
{
	GLfloat positions[8*3] =
	{
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f
	};

	GLuint indices[12*3] =
	{
		5, 0, 1,
		5, 4, 0,
		2, 0, 3,
		2, 1, 0,
		7, 0, 4,
		7, 3, 0,
		3, 6, 2,
		3, 7, 6,
		1, 2, 6,
		1, 6, 5,
		4, 5, 6,
		4, 6, 7
	};

	const char files[6][30] =
	{
			"skybox/posx.jpg",
			"skybox/negx.jpg",
			"skybox/posy.jpg",
			"skybox/negy.jpg",
			"skybox/posz.jpg",
			"skybox/negz.jpg",

	};

	const GLenum targets[6] =
	{
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	// Program
	SkyBox_Program = glCreateProgram();
	glAttachShader( SkyBox_Program, LoadShader(GL_VERTEX_SHADER, "SkyBox-vertex.glsl"));
	glAttachShader( SkyBox_Program, LoadShader(GL_FRAGMENT_SHADER, "SkyBox-fragment.glsl"));
	LinkAndValidateProgram( SkyBox_Program );

	// Vertex arrays
	glGenVertexArrays( 1, &SkyBox_VAO );
	glBindVertexArray( SkyBox_VAO );

	// Wspolrzedne wierzchokow
	GLuint vBuffer_pos;
	glGenBuffers( 1, &vBuffer_pos );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos );
	glBufferData( GL_ARRAY_BUFFER, 8*3*sizeof(GLfloat), positions, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	// Tablica indeksow
	GLuint vBuffer_idx;
	glGenBuffers( 1, &vBuffer_idx );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vBuffer_idx );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 12*3*sizeof( GLuint ), indices, GL_STATIC_DRAW );

	glBindVertexArray( 0 );

	// Tekstura CUBE_MAP
    glGenTextures( 1, &SkyBox_Texture);
	glBindTexture( GL_TEXTURE_CUBE_MAP, SkyBox_Texture );

	// Utworzenie 6 tekstur dla kazdej sciany
	for (int i=0; i < 6; ++i)
	{
		int tex_width, tex_height, n;
		unsigned char *tex_data;

    	tex_data = stbi_load(files[i], &tex_width, &tex_height, &n, 0);
    	if (tex_data == NULL)
		{
			printf("Image %s can't be loaded!\n", files[i]);
			exit(1);
		}
		// Zaladowanie danych do tekstury OpenGL
		glTexImage2D( targets[i], 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);

		// Zwolnienie pamieci pliku graficznego
		stbi_image_free(tex_data);

	}

	// Przykladowe opcje tekstury
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT );
	glGenerateMipmap( GL_TEXTURE_CUBE_MAP );
	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );

}


// ---------------------------------------------------
void Initialize()
{
	// ----------------------------------------
	// Tworzenie Sky Box'a
	// ----------------------------------------
	CreateSkyBox();



	// ----------------------------------------
	// Utworzenie domyslnego programu i obiektu
	// ----------------------------------------
	{
		// Ladowanie pliku OBJ
		if (!loadOBJ("koliber.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
		{
			printf("Not loaded!\n");
			exit(1);
		}


		// Ladowanie pliku tekstury
		int tex_width, tex_height, n;
		unsigned char *tex_data;

		tex_data = stbi_load("koliber.bmp", &tex_width, &tex_height, &n, 0);
		if (tex_data == NULL)
		{
			printf("Image %s can't be loaded!\n", "multi_texture.bmp");
			exit(1);
		}

		// Tworzenie obiektu tekstury
		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Zwolnienie wczytanej tekstury z pamieci RAM
		stbi_image_free(tex_data);


		// Programowanie potoku
		program = glCreateProgram();
		glAttachShader( program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
		glAttachShader( program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
		LinkAndValidateProgram( program );


		// Tablica wierzcholkow
		glGenVertexArrays( 1, &vArray );
		glBindVertexArray( vArray );

		// Wspolrzedne wierzchokow
		GLuint vBuffer_pos;
		glGenBuffers( 1, &vBuffer_pos );
		glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos );
		glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 0 );

		// Wspolrzedne textury UV
		GLuint vBuffer_uv;
		glGenBuffers( 1, &vBuffer_uv );
		glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv );
		glBufferData( GL_ARRAY_BUFFER, OBJ_uvs.size() * sizeof(glm::vec2), &OBJ_uvs[0], GL_STATIC_DRAW );
		glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 1 );

		// Wektory normalne
		GLuint vBuffer_normal;
		glGenBuffers( 1, &vBuffer_normal );
		glBindBuffer( GL_ARRAY_BUFFER, vBuffer_normal );
		glBufferData( GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW );
		glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 2 );
	}



	// ----------------------------------------
	// Inne ustawienia OpenGL
	// ----------------------------------------

	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
	glEnable( GL_DEPTH_TEST );


}

// ---------------------------------------------------
enum { MENU_END = 0, };
void Menu( int value )
{
    switch( value )
    {

	case MENU_END:
		glutLeaveMainLoop();
		break;
    }

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

	int podmenuA = glutCreateMenu( Menu );
    glutAddMenuEntry( "Pozycja 1", 1 );
    glutAddMenuEntry( "Koniec", MENU_END );
    glutAttachMenu( GLUT_RIGHT_BUTTON );


	glutMainLoop();


	return 0;
}
