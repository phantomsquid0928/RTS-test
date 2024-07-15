#include "block.h"
#include "headers.hpp"

block::block() {

}
block::block(const vector<array<int, 2>>& positions) { //block default size is 1, position 0, 0
	vcode = readFile("shader/block_vertex.glsl"); //modify by classes
	fcode = readFile("shader/block_fragment.glsl");
	shaderProgram = createShaderProgram(vcode.c_str(), fcode.c_str());
	cout << "block shader : " << shaderProgram << endl;

	size = 1;

	this->positions = &positions;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
}
void block::render() { //render func for blocks (wall)
	glUseProgram(shaderProgram);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_LINE_LOOP, 0, 4);

	glBindVertexArray(0);
	glUseProgram(0);
}


void block::create() { //bulk loading blocktypes (wall ...)
	vertices.clear();
	indices.clear();

	int i = 0;
	for (auto p : *positions) {
		float x1 = (2.0f * p[0]) / sizex - 1.0f;
		float y1 = 1.0f - (2.0f * p[1]) / sizey;
		float x2 = (2.0f * (p[0] + size)) / sizex - 1.0f;
		float y2 = 1.0f - (2.0f * (p[1] + size)) / sizey;

		vertices.insert(vertices.end(), {
			x1, y1, 0.0f,
			x2, y1, 0.0f,
			x2, y2, 0.0f,
			x1, y2, 0.0f
		});

		unsigned int base = i * 4;
		indices.insert(indices.end(), {
			base, base + 1, base + 2,
			base + 2, base + 3, base
			});
		i++;
	}

	//generate buffers

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void block::update() {
	int i = 0;
	vertices.clear();
	indices.clear();
	for (auto p : *positions) {
		float x1 = (2.0f * p[0]) / sizex - 1.0f;
		float y1 = 1.0f - (2.0f * p[1]) / sizey;
		float x2 = (2.0f * (p[0] + size)) / sizex - 1.0f;
		float y2 = 1.0f - (2.0f * (p[1] + size)) / sizey;

		vertices.insert(vertices.end(), {
			x1, y1, 0.0f,
			x2, y1, 0.0f,
			x2, y2, 0.0f,
			x1, y2, 0.0f
			});

		unsigned int base = i * 4;
		indices.insert(indices.end(), {
			base, base + 1, base + 2,
			base + 2, base + 3, base
			});
		i++;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
mat4 block::getaabb() {
	return mat4(1.0f);
}