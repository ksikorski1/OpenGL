#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "shader_stuff.hpp"
#include "object3d.hpp"

// ---------------------------------------
// NOWE : OSWIETLENIE - parametry
// ---------------------------------------



SLight Light1 = {
	glm::vec4(0.3, 0.3, 0.3, 1.0),
	glm::vec4(0.8, 0.8, 0.8, 1.0),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	glm::vec4(0.0, 3.0, 0.0, 1.0),
	0.01f,
};


// ---------------------------------------
// NOWE : MATERIALY - parametry
// ---------------------------------------
typedef struct SMaterial
{

	glm::vec4 Ambient;
    glm::vec4 Diffuse;
    glm::vec4 Specular;
    GLfloat Shininess;

} SMaterial;


SMaterial Material1 = {
	glm::vec4(0.2f, 0.8f, 0.2f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
	32.0f
};

SMaterial Material2 = {
	glm::vec4(1.0f, 0.2f, 0.2f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
	32.0f
};



// ---------------------------------------
GLuint idProgram;

// ---------------------------------------
// Macierze przeksztalcen i rzutowania
// ---------------------------------------
glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matProjViewModel;

glm::vec3 Camera_Position;

// ---------------------------------------
// Obiekty na scenie
// ---------------------------------------
CObject Scene, Tree;


float rotation = 0.0f;
float light_rotation = 0.0f;
// ---------------------------------------
void DisplayScene()
{
	// Czyszczenie ramki
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // ------------------------------
	// Geometria sceny
	matView = glm::mat4x4( 1.0 );
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	Camera_Position = ExtractCameraPos(matView);


	// 1. Wlaczanie programu
	glUseProgram(idProgram);

		// 2. Wysylanie parametrow sceny

		// (a) pozycja kamery
		glUniform3fv( glGetUniformLocation( idProgram, "Camera_Position" ), 1, &Camera_Position[0] );

		//Light1.Position = glm::vec4(Camera_Position, 1.0f);

		//Animacja pozycji oswietlenia
		{
		    light_rotation += 0.05f;

		    glm::mat4 matModelLight = glm::mat4(1.0f);
            matModelLight = glm::rotate(matModelLight, light_rotation, glm::vec3(0.0, 1.0, 0.0));
            matModelLight = glm::translate(matModelLight, glm::vec3(5.0, 0.0, 0.0));

            glm::vec4 LightBasePosition = glm::vec4(0.0, 3.0, 0.0, 1.0);
            Light1.Position = matModelLight * LightBasePosition;
		}

		// (b) oswietlenie
		glUniform4fv( glGetUniformLocation(idProgram, "myLight.Ambient"), 1, &Light1.Ambient[0]);
		glUniform4fv( glGetUniformLocation(idProgram, "myLight.Diffuse"), 1, &Light1.Diffuse[0]);
		glUniform4fv( glGetUniformLocation(idProgram, "myLight.Specular"), 1, &Light1.Specular[0]);
		glUniform4fv( glGetUniformLocation(idProgram, "myLight.Position"), 1, &Light1.Position[0]);
		glUniform1f( glGetUniformLocation(idProgram, "myLight.Attenuation"), Light1.Attenuation);


		// 3. Renderowanie obiektow

		// Pierwszy obiekt
		{
			// (a) material
			glUniform4fv( glGetUniformLocation(idProgram, "myMaterial.Ambient"), 1, &Material1.Ambient[0]);
			glUniform4fv( glGetUniformLocation(idProgram, "myMaterial.Diffuse"), 1, &Material1.Diffuse[0]);
			glUniform4fv( glGetUniformLocation(idProgram, "myMaterial.Specular"), 1, &Material1.Specular[0]);
			glUniform1f( glGetUniformLocation(idProgram, "myMaterial.Shininess"), Material1.Shininess);

			// (b) macierz rzutowania
			glm::mat4 matModel = glm::mat4(1.0);
			matProjViewModel = matProj * matView * matModel;

			glm::mat3 matNormal = glm::transpose(glm::inverse(glm::mat3(matModel)));

			glUniformMatrix4fv( glGetUniformLocation( idProgram, "matProjViewModel" ), 1, GL_FALSE, &(matProjViewModel[0][0]) );
			glUniformMatrix4fv( glGetUniformLocation( idProgram, "matModel" ), 1, GL_FALSE, &(matModel[0][0]) );
			glUniformMatrix3fv( glGetUniformLocation( idProgram, "matNormal" ), 1, GL_FALSE, &(matNormal[0][0]) );


			// (c) Renderowanie
			Scene.Draw();
		}
		rotation += 0.05;

		// Drugi obiekt
		{
			// (a) material
			glUniform4fv( glGetUniformLocation(idProgram, "myMaterial.Ambient"), 1, &Material2.Ambient[0]);
			glUniform4fv( glGetUniformLocation(idProgram, "myMaterial.Diffuse"), 1, &Material2.Diffuse[0]);
			glUniform4fv( glGetUniformLocation(idProgram, "myMaterial.Specular"), 1, &Material2.Specular[0]);
			glUniform1f( glGetUniformLocation(idProgram, "myMaterial.Shininess"), Material2.Shininess);

			// (b) macierz rzutowania
			glm::mat4 matModel = glm::translate(glm::mat4(1.0), glm::vec3(2.0, 0.0, 2.0));
			matModel = glm::rotate(matModel, rotation, glm::vec3(0.5, 1.0, 0.1));
			matProjViewModel = matProj * matView * matModel;
			glm::mat3 matNormal = glm::transpose(glm::inverse(glm::mat3(matModel)));
			glUniformMatrix4fv( glGetUniformLocation( idProgram, "matProjViewModel" ), 1, GL_FALSE, &(matProjViewModel[0][0]) );
            glUniformMatrix4fv( glGetUniformLocation( idProgram, "matModel" ), 1, GL_FALSE, &(matModel[0][0]) );
            glUniformMatrix3fv( glGetUniformLocation( idProgram, "matNormal" ), 1, GL_FALSE, &(matNormal[0][0]) );

			// (c) Renderowanie
			Tree.Draw();
		}


	// 4. Wylaczanie programu
	glUseProgram( 0 );



    glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
    glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}


// ---------------------------------------------------
void Initialize()
{

	// Programowanie potoku
	idProgram = glCreateProgram();

	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram( idProgram );

	glEnable( GL_DEPTH_TEST );
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );


	// Ladowanie obiektow
	Scene.Load("scene.obj");
	Tree.Load("object.obj");

}

// ---------------------------------------------------
void Animation(int frame)
{
	glutPostRedisplay();
	glutTimerFunc(10, Animation, 0);
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

	glutTimerFunc(10, Animation, 0);

    glutMainLoop();

    // Cleaning();
    glDeleteProgram( idProgram );

    return 0;
}
