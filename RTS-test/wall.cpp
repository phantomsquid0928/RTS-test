
#include "wall.h"
#include "headers.hpp"

using namespace mapinfo;
wall::wall() { //block default size is 1, position 0, 0
	vcode = readFile("shader/block_vertex.glsl"); //modify by classes
	fcode = readFile("shader/block_fragment.glsl");
	shaderProgram = createShaderProgram(vcode.c_str(), fcode.c_str());
	cout << "block shader : " << shaderProgram << endl;

	size = 1;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
}
void wall::render() { //render func for walls (wall)
	glUseProgram(shaderProgram);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_LINE_LOOP, 0, 4);

	glBindVertexArray(0);
	glUseProgram(0);
}


void wall::create() { //bulk loading walltypes (wall ...)
	vertices.clear();
	indices.clear();

	int cnt = 0;
	for (int i = 0; i < sizey; i++) {
		for (int j = 0; j < sizex; j++) {
			if (arr[i][j] != 1) continue;
			float x1 = (2.0f * i) / sizex - 1.0f;
			float y1 = 1.0f - (2.0f * j) / sizey;
			float x2 = (2.0f * (i + size)) / sizex - 1.0f;
			float y2 = 1.0f - (2.0f * (j + size)) / sizey;
			vertices.insert(vertices.end(), {
				x1, y1, 0.0f,
				x2, y1, 0.0f,
				x2, y2, 0.0f,
				x1, y2, 0.0f
				});
			unsigned int base = cnt * 4;
			indices.insert(indices.end(), {
				base, base + 1, base + 2,
				base + 2, base + 3, base
				});
			cnt++;
		}
	}

	//generate buffers

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void wall::update() {
	vertices.clear();
	indices.clear();

	int cnt = 0;
	for (int i = 0; i < sizey; i++) {
		for (int j = 0; j < sizex; j++) {
			if (arr[i][j] != 1) continue;
			float x1 = (2.0f * i) / sizex - 1.0f;
			float y1 = 1.0f - (2.0f * j) / sizey;
			float x2 = (2.0f * (i + size)) / sizex - 1.0f;
			float y2 = 1.0f - (2.0f * (j + size)) / sizey;
			vertices.insert(vertices.end(), {
				x1, y1, 0.0f,
				x2, y1, 0.0f,
				x2, y2, 0.0f,
				x1, y2, 0.0f
				});
			unsigned int base = cnt * 4;
			indices.insert(indices.end(), {
				base, base + 1, base + 2,
				base + 2, base + 3, base
				});
			cnt++;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
mat4 wall::getaabb() {
	return mat4(1.0f);
}