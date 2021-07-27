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
glm::mat4 matView2;
glm::mat4 matView3;
glm::mat4 matView4;
glm::mat4 matProjView;


// Identyfikatory obiektow


class OpenGLProgram
{
    public:
        glm::mat4 ProjectionMatrix;
        GLuint idProgram;

        GLuint idVAO;
        GLuint idVBO_coord;

        std::vector<glm::vec3> OBJ_vertices;
        std::vector<glm::vec2> OBJ_uvs;
        std::vector<glm::vec3> OBJ_normals;

        void CreateProgram(){
            idProgram = glCreateProgram();
        }

        void loadShaders(char* vertex_filename, char* fragment_filename){
            glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, vertex_filename));
            glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, fragment_filename));

            LinkAndValidateProgram( idProgram );
        }

        void ImportOBJ(char* filename){


            if (!loadOBJ(filename, OBJ_vertices, OBJ_uvs, OBJ_normals))
            {
                printf("Not loaded!\n");
                exit(1);
            }
            CreateVBO();
        }

        void CreateVBO(){
            glGenVertexArrays( 1, &idVAO );
            glBindVertexArray( idVAO );

            // Bufor na wspolrzedne wierzcholkow
            glGenBuffers( 1, &idVBO_coord );

            glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
            glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );

            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
            glEnableVertexAttribArray( 0 );



            glBindVertexArray( 0 );
        }

        void Draw(){
            glBindVertexArray( idVAO );
            glUseProgram( idProgram );

            glUniformMatrix4fv( glGetUniformLocation(idProgram, "matProjView"), 1, GL_FALSE,  glm::value_ptr(ProjectionMatrix));
            // Generowanie obiektow na ekranie
            glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );

            glUseProgram( 0 );
            glBindVertexArray( 0 );
        }

        void SetModelProjectionMatrix(glm::mat4 matrix){
            ProjectionMatrix = matrix;
            //glUniformMatrix4fv( glGetUniformLocation(idProgram, "matProjView"), 1, GL_FALSE,  glm::value_ptr(matrix));
        }
};

OpenGLProgram scena;
OpenGLProgram malpa;
OpenGLProgram niebo;
OpenGLProgram wieza;

// ---------------------------------------
void DisplayScene()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Macierz widoku
	matView = glm::mat4x4( 1.0 );
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    matView2 = matView3 = matView4 = matView;

	// Obliczanie macierzy rzutowania
	matView = glm::scale(matView, glm::vec3(1.1f, 1.1f, 1.1f));
	matProjView = matProj * matView;
    scena.SetModelProjectionMatrix(matProjView);
	scena.Draw();

    matView2 = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y+5, _scene_translate_z+9) );
    matProjView = matProj * matView2;
	malpa.SetModelProjectionMatrix(matProjView);
	malpa.Draw();

    matView3 = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z+30) );
    matView3 = glm::scale(matView3, glm::vec3(15.0f, 15.0f, 15.0f));
    matProjView = matProj * matView3;
	niebo.SetModelProjectionMatrix(matProjView);
	niebo.Draw();

	matView4 = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y-2, _scene_translate_z+7) );
    matProjView = matProj * matView4;
	wieza.SetModelProjectionMatrix(matProjView);
	wieza.Draw();

	glutSwapBuffers();

}

// ---------------------------------------
void Initialize()
{

    scena.CreateProgram();
    scena.loadShaders("vertex.glsl", "fragment.glsl");
	scena.ImportOBJ("scene.obj");

	malpa.CreateProgram();
    malpa.loadShaders("vertex-malpa.glsl", "fragment-malpa.glsl");
	malpa.ImportOBJ("monkey.obj");

	niebo.CreateProgram();
    niebo.loadShaders("vertex-niebo.glsl", "fragment-niebo.glsl");
	niebo.ImportOBJ("sky.obj");

	wieza.CreateProgram();
    wieza.loadShaders("vertex-wieza.glsl", "fragment-wieza.glsl");
	wieza.ImportOBJ("tower.obj");


	// ustawienie poczatkowej translacji na z
	_scene_translate_z = -10.0f;

	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	glEnable(GL_DEPTH_TEST);
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


	glutMainLoop();


	// Cleaning


	return 0;
}
