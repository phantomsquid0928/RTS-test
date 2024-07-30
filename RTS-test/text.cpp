#include "text.h"

text::text() {
	vcode = readFile("shader/entity_vertex.glsl"); //mod
	fcode = readFile("shader/entity_fragment.glsl");

	shaderProgram = createShaderProgram(vcode.c_str(), fcode.c_str());

	cout << "text shader : " << shaderProgram << endl;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
}
void text::addText(string text) {
	texts.push_back(text);
	this->update();
}
void text::changeText(int idx, string text) {
	texts[idx] = text;
	this->update();
}
void text::create() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void text::render() {
	glUseProgram(shaderProgram);
	glUseProgram(0);
}
void text::update() {
	for (auto& t : texts) {
		s
	}
}
