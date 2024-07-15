#include "shader.h"

shader::shader() {
	VAO = 0; VBO = 0; IBO = 0;
	shaderProgram = 0;
}
GLuint shader::compile(GLenum type, const char* source) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return shader;
}

GLuint shader::createShaderProgram(const char* vcode, const char* fcode) {
	GLuint vertexShader = compile(GL_VERTEX_SHADER, vcode);
	GLuint fragmentShader = compile(GL_FRAGMENT_SHADER, fcode);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	GLint success;
	GLchar infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

GLuint shader::changeFshader(const char* fcode) { //not perfect
	GLuint fragmentShader = compile(GL_FRAGMENT_SHADER, fcode);

	glAttachShader(shaderProgram, fragmentShader);

	return shaderProgram;
}
string shader::readFile(string filepath) {
	ifstream file(filepath);
	stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void shader::create() {

}
/// <summary>
/// childs should override these funcs below...
/// </summary>
void shader::render() {
	cout << "no";
}
/// <summary>
/// childs should override these funcs below...
/// </summary>
void shader::update() {
	cout << "non";
}
void shader::updateAll() {

}
void shader::destroy() {

}