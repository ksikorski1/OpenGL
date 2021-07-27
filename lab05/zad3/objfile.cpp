#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_stuff.hpp"
#include "obj_loader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


glm::mat4 matProj;
glm::mat4 matView;

// Identyfikatory obiektow


class OpenGLProgram
{
    public:
        glm::mat4 ProjectionMatrix;
        GLuint idProgram;

        GLuint idVAO;
        GLuint idVBO_coord;
        GLuint idVBO_normal;
        GLuint idVBO_uv;

        glm::mat4 matModel = glm::mat4x4( 1.0 );

        std::vector<glm::vec3> OBJ_vertices;
        std::vector<glm::vec2> OBJ_uvs;
        std::vector<glm::vec3> OBJ_normals;

        GLuint TextureID;
        int tex_width;
        int tex_height;
        int n;
        unsigned char *data;

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


            glGenBuffers( 1, &idVBO_normal );
            glBindBuffer( GL_ARRAY_BUFFER, idVBO_normal );
            glBufferData( GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW );
            glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
            glEnableVertexAttribArray( 1 );


            glGenBuffers( 1, &idVBO_uv );
            glBindBuffer( GL_ARRAY_BUFFER, idVBO_uv );
            glBufferData( GL_ARRAY_BUFFER, OBJ_uvs.size() * sizeof(glm::vec2), &OBJ_uvs[0], GL_STATIC_DRAW );
            glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, NULL );
            glEnableVertexAttribArray( 2 );


            glBindVertexArray( 0 );
        }

        void Draw(){
            glBindVertexArray( idVAO );
            glUseProgram( idProgram );

                glUniformMatrix4fv( glGetUniformLocation(idProgram, "matProjView"), 1, GL_FALSE,  glm::value_ptr(ProjectionMatrix));
                // Generowanie obiektow na ekranie
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, TextureID);

                // NOWE: Przekazanie identyfikatora samplera
                glUniform1i(glGetUniformLocation(idProgram, "tex0"), 0);
                glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );
                //glDrawElements(GL_TRIANGLES, OBJ_vertices.size(), GL_UNSIGNED_INT, NULL);

            glUseProgram( 0 );
            glBindVertexArray( 0 );
        }
        void SetProjViewMatrix(){
            ProjectionMatrix = matProj * matView * matModel;
        }

        void CreateTexture(char* filename){

            stbi_set_flip_vertically_on_load(true);

            data = stbi_load(filename, &tex_width, &tex_height, &n, 0);

            glGenTextures(1, &TextureID);
            glBindTexture(GL_TEXTURE_2D, TextureID);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

            // GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

            // (b) Zachowanie tekstury w przypadku powiekszenia

            // GL_LINEAR, GL_NEAREST
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // (c) Zachowanie tekstury w przypadku pomniejszenia

            // GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST,
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

            // Generowanie mipmap automatycznie
            glGenerateMipmap(GL_TEXTURE_2D);

            // Podstawowy level mipmap (default 0)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        }

        void move(float X = 0.0f, float Y = 0.0f, float Z = 0.0f){
            matModel = glm::translate( matModel, glm::vec3( X, Y, Z) );
        }

        void rotateX(float angle){
            matModel = glm::rotate( matModel, angle, glm::vec3( 1.0f, 0.0f, 0.0f ) );
        }
        void rotateY(float angle){
            matModel = glm::rotate( matModel, angle, glm::vec3( 0.0f, 1.0f, 0.0f ) );
        }
        void rotateZ(float angle){
            matModel = glm::rotate( matModel, angle, glm::vec3( 0.0f, 0.0f, 1.0f ) );
        }
        void scale(float X = 0.0f, float Y = 0.0f, float Z = 0.0f){
            matModel = glm::scale(matModel, glm::vec3(X, Y, Z));
        }
};

OpenGLProgram scena;
OpenGLProgram tree;
OpenGLProgram tree2;
OpenGLProgram niebo;
OpenGLProgram wieza;
OpenGLProgram cottage;

// ---------------------------------------
void DisplayScene()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Macierz widoku
	matView = glm::mat4x4( 1.0 );
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );

	scena.SetProjViewMatrix();
	scena.Draw();

	tree.SetProjViewMatrix();
	tree.Draw();

	tree2.SetProjViewMatrix();
	tree2.Draw();

    niebo.SetProjViewMatrix();
	niebo.Draw();

	wieza.SetProjViewMatrix();
	wieza.Draw();

	cottage.SetProjViewMatrix();
	cottage.Draw();

	glutSwapBuffers();

}

// ---------------------------------------
void Initialize()
{

    scena.CreateProgram();
    scena.loadShaders("vertex.glsl", "fragment.glsl");
	scena.ImportOBJ("ground.obj");
    scena.CreateTexture("41832706361b9823c8cco_swnqu.jpg");
    scena.scale(12.0, 12.0, 12.0);

	tree.CreateProgram();
    tree.loadShaders("vertex-tree.glsl", "fragment-tree.glsl");
	tree.ImportOBJ("tree.obj");
    tree.CreateTexture("Seamlessgrasstexturebyhhh316_ovcfd.jpg");
    tree.move(8.0, -0.0, -5.0);

    tree2.CreateProgram();
    tree2.loadShaders("vertex-tree.glsl", "fragment-tree.glsl");
	tree2.ImportOBJ("tree.obj");
    tree2.CreateTexture("Seamlessgrasstexturebyhhh316_ovcfd.jpg");
    tree2.move(8.0, 0.0, 10.0);

	niebo.CreateProgram();
    niebo.loadShaders("vertex-niebo.glsl", "fragment-niebo.glsl");
	niebo.ImportOBJ("sky.obj");
	niebo.CreateTexture("sky.jpg");
	niebo.scale(20.0, 20.0, 20.0);

	wieza.CreateProgram();
    wieza.loadShaders("vertex-wieza.glsl", "fragment-wieza.glsl");
	wieza.ImportOBJ("tower.obj");
	wieza.CreateTexture("metal.jpg");
	wieza.move(-3.0, 0.0, -10.0);

	cottage.CreateProgram();
    cottage.loadShaders("vertex-cottage.glsl", "fragment-cottage.glsl");
	cottage.ImportOBJ("cottage.obj");
	cottage.CreateTexture("1920x1080.png");
	cottage.move(15.0, 0.0, 12.0);
	cottage.rotateY(-5.0);
	cottage.scale(0.3, 0.3, 0.3);


	// ustawienie poczatkowej translacji na z
	_scene_translate_z = -10.0f;
	_scene_translate_y = -3.0f;

	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	glEnable(GL_DEPTH_TEST);
}

// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );

	matProj = glm::ortho(-1.0, 5.0, -1.0, 1.0);
	matProj = glm::perspective(glm::radians(80.0f), width/(float)height, 0.1f, 80.0f);

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
