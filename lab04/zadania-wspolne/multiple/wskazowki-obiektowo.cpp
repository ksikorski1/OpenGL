// Wskazówki do stworzenia obiektowej obs³ugi programów OpenGL


// 1. Chcemy stworzyc klase np. COpenGLProgram z metodami:

class COpenGLProgram
{

	// Metoda tworzaca program, jego identyfikator
	// Moglaby od razu stworzyc VAO
	CreateProgram();

	// Metody ladujace shadery:

	LoadVertexShader(filename);
	LoadFragmentShader(filename);

	// lub w jednej metodzie

	LoadShaders(vertex_filename, fragment_filename);


	// Metody tworzace VBO oraz kopiujace tam dane
	CreateVBO(list_of_vertex)


	// Metody ladujaca plik obj ktora od razu wrzuca dane do VBO
	ImportOBJ(filename)


	// Oczywiscie metoda generujaca obraz na ekranie
	Draw();

	// Byc moze jakas metoda przekazujaca macierz rzutowania do programu
	SetModelviewProjectionMatrix(matrix);

	// Albo inne przekazujaca dodatkowe zmienne uniform


};


// 2. Majac taka klase bardzo wygodnie byloby dodawac i generowac na ekranie obiekty

// Przykladowo:  majac obiekt

COpenGLProgram mojProgram;





// ---------------------------------------
void DisplayScene()
{
	// Czyszczenie ramki
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	// Geometria sceny i obliczanie macierzy rzutowania
	Matrix_mv = glm::mat4x4( 1.0 );
	...
	Matrix_proj_mv = Matrix_proj * Matrix_mv;



	// Przekazanie macierzy rzutowania do programu
	mojProgram.SetModelviewProjectionMatrix(Matrix_proj_mv);
	// I wygenerowanie na ekranie
	mojProgram.Draw();





	glutSwapBuffers();
}

// ---------------------------------------------------
void Initialize()
{

	// Tworzenie programu
	mojProgram.CreateProgram();

	// Ladowanie shaderow
	mojProgram.LoadShaders("vertex.glsl", "fragment.glsl");

	// Zaladowanie obiektu z pliku OBJ oraz utworzenie VAO i VBO
	mojProgram.ImportOBJ("ground.obj");


}


// Majac taka klase bedzie nam duuzo wygodniej w przyszlosci
