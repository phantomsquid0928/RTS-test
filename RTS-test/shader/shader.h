#pragma once
#include "../headers.hpp"

class shader {
public:
	shader();
	GLuint createShaderProgram(const char* vcode, const char* fcode);
	GLuint changeFshader(const char* fcode);
	string readFile(string filepath);
	virtual void create();
	virtual void render();
	virtual void update();
	virtual void updateAll();
	virtual void destroy();
	GLuint VAO, VBO, IBO;
	GLuint shaderProgram;
	string vcode;
	string fcode;
private:
	GLuint compile(GLenum type, const char* source);
};