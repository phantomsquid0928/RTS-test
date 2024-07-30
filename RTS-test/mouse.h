#pragma once
#include "../shader/shader.h"

class mouse : shader
{
public:
	GLuint VAO, VBO, IBO;
	GLuint shaderProgram;
	void update();
	void render();
	mouse();
};

