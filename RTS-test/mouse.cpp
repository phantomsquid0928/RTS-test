#include "mouse.h"

using namespace input;
float vertice[12] = { 0.0f,  0.0f, 0.0f,  // Bottom left
			 1.0f,  0.0f, 0.0f,  // Bottom right
			 1.0f,  1.0f, 0.0f,  // Top right
			 0.0f,  1.0f, 0.0f   // Top left
};
mouse::mouse() {
	vcode = readFile("mouse_vertex.glsl"); //mod
	fcode = readFile("mouse_fragment.glsl");
	shaderProgram = createShaderProgram(vcode.c_str(), fcode.c_str());
	cout << "mouse shader : " << shaderProgram << endl;
}
void mouse::render() { //TODO: need to assume shaderProgram called.
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	unsigned int indices[] = {
	   0, 1, 2,
	   2, 3, 0
	};

	

	glBindVertexArray(VAO);


	// VBO ���ε� �� ������ ����
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_DYNAMIC_DRAW);

	// EBO ���ε� �� ������ ����
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// ���� �Ӽ� ������ ����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ���ε� ����
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	cout << VAO << VBO << IBO;
}
void mouse::update() {
	if (mouse_status[leftmouse] == DRAGGING) {
		// �簢�� ũ�� ������Ʈ
		float x1 = (2.0f * clickposx) / sizex - 1;
		float y1 = 1.0f - (2.0f * clickposy) / sizey;
		float x2 = (2.0f * mousex) / sizex - 1;
		float y2 = 1.0f - (2.0f * mousey) / sizey;

		vertice[0] = x1; vertice[1] = y1; // Bottom left
		vertice[3] = x2; vertice[4] = y1; // Bottom right
		vertice[6] = x2; vertice[7] = y2; // Top right
		vertice[9] = x1; vertice[10] = y2; // Top left

		// VAO, VBO ������Ʈ
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertice), vertice);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		//cout << shaderProgram << endl;
		// ���̴� ���α׷� ���
		glUseProgram(shaderProgram);

		// VAO ���ε� �� �׸���
		glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_LINE_LOOP, 0, 4);

		glBindVertexArray(0);
		glUseProgram(0);
	}
}