#ifndef __OBJECT_3D
#define __OBJECT_3D


class CObject
{

public:

	std::vector<glm::vec3> OBJ_vertices;
	std::vector<glm::vec2> OBJ_uvs;
	std::vector<glm::vec3> OBJ_normals;

	GLuint vArray;
	GLuint Size;

	CObject() { }

	void Draw()
	{
		glBindVertexArray( vArray );
		glDrawArrays( GL_TRIANGLES, 0, Size );
		glBindVertexArray( 0 );
	}

	// ---------------------------------------
	void Load(char *filename)
	{

		if (!loadOBJ(filename, OBJ_vertices, OBJ_uvs, OBJ_normals))
		{
			printf("Not loaded!\n");
			exit(1);
		}

		// Liczba wierzcholkow potrzebna do wyrenderowania obiektu
		Size = OBJ_vertices.size();

		// Vertex arrays
		glGenVertexArrays( 1, &this->vArray );
		glBindVertexArray( this->vArray );

		// Wspolrzedne wierzchokow
		GLuint vBuffer_coord;
		glGenBuffers( 1, &vBuffer_coord );
		glBindBuffer( GL_ARRAY_BUFFER, vBuffer_coord );
		glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );

		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 0 );

		// Wektory normalne
		GLuint vBuffer_normal;
		glGenBuffers( 1, &vBuffer_normal );
		glBindBuffer( GL_ARRAY_BUFFER, vBuffer_normal );
		glBufferData( GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW );

		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 1 );

		glBindVertexArray( 0 );




	}

};


#endif
