#pragma once
#include "../headers.hpp"

class shader {
public:
	shader();
	GLuint createShaderProgram(const char* vcode, const char* fcode);
	string readFile(string filepath);
	void render();
	void update();
	GLuint VAO, VBO, IBO;
	GLuint shaderProgram;
	string vcode;
	string fcode;
private:
	GLuint compile(GLenum type, const char* source);
	int vshader, fshader;
};