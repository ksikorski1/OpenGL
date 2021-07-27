#ifndef __CSCENE_OBJECT
#define __CSCENE_OBJECT

// ----------------------------------------------------------------
// Klasa do reprezentacji obiektu, ktory porusza sie po scenie
class CSceneObject
{

public:

	// pozycja obiektu na scenie
    glm::vec3 Position;
	glm::mat4 matModel;

	// potok openGL
    GLuint VAO;
    int VBO_Size;

    // Podlaczenie do CGround
	CGround *Ground = NULL;


    CSceneObject() { }

    // Inicjalizacja obiektu
    void Init(GLuint _vao, int _size, CGround *_ground)
    {
        VAO = _vao;
        VBO_Size = _size;
        Ground = _ground;
    }

	// Obliczenie wysokosci nad ziemia
	void UpdateLatitute()
	{
		float newAlt = Ground->getAltitute(glm::vec2(Position.x, Position.z));

		Position.y = newAlt;
		matModel = glm::translate(glm::mat4(1.0), Position);
	}

    // Rysowanie na scenie
    void Draw()
    {

		GLint current_program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);

        glUseProgram(current_program);

        glUniformMatrix4fv( glGetUniformLocation( current_program, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );

        glBindVertexArray( VAO );
        glDrawArrays( GL_TRIANGLES, 0, VBO_Size );
        glBindVertexArray( 0 );

    }

    // ustawienie pozycji na scenie
    void SetPosition(float x, float y, float z)
    {
        Position = glm::vec3(x, y, z);
        matModel = glm::translate(glm::mat4(1.0), Position);
    }

    // zmiana pozycji na scenie
    void MoveXZ(float _x, float _z)
    {
        Position += glm::vec3(_x, 0.0, _z);

        // Aktualizacja polozenia na Y
        UpdateLatitute();

    }

};


#endif
