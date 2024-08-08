#include "bbox.h"
#include "headers.hpp"


using namespace input;
using namespace mapinfo;
//line юс
//latest version of rendering objects
bbox::bbox(vector<vector<point>> * boxes) {
	vcode = readFile("shader/entity_vertex.glsl"); //mod
	fcode = readFile("shader/entity_fragment.glsl");

	shaderProgram = createShaderProgram(vcode.c_str(), fcode.c_str());

	cout << "entity shader : " << shaderProgram << endl;
	this->boxes = boxes;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
}

bbox::~bbox() {
}

void bbox::create() { //TODO : refactoring of vertices, indices needed
	//generate buffers
	vertices.clear();
	indices.clear();
	unsigned int base = 0;
	for (auto & i : *boxes) {
		int num = 0;

		for (auto &k : i) {
			vertices.insert(vertices.end(), {
				(float)abs(k.y - i[0].y), (float)abs(k.x - i[0].x), 0
				});
			num++;
		}
		int trim = 0;

		indices.insert(indices.end(), {
			base, base + 1, base + 2,
			base + 2, base + 3, base
			});
		
		base += num;
	}

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
void bbox::render() { //TODO: need to assume shaderProgram called.
	//glUseProgram(shaderProgram);	
	//glDrawArrays(GL_LINE_LOOP, 0, 4);

	glUseProgram(shaderProgram);
	int i = 0;
	int j = 0;
	int cnt = boxes->size();
	for (auto a : *boxes) {
		vec2 position = vec2(a[0].x, a[0].y); 

		mat4 model = mat4(1.0f);
		mat4 project = mat4(1.0f);


		//model = translate(model, vec3(2 * entitylist[i][0] / sizex - 1, 1 - 2 * entitylist[i][1] / sizey, 0));
		model = translate(model, vec3(position.y, position.x, 0));

		project = ortho(0.0f, (float)sizey, (float)sizex, 0.0f, -1.0f, 1.0f);

		GLuint colorizer = glGetUniformLocation(shaderProgram, "color");
		/*if (selected[i]) {
			glUniform4f(colorizer, 0.0f, 1.0f, 0.0f, 1.0f);
		}
		else {
			glUniform4f(colorizer, 0.0f, 0.0f, 1.0f, 1.0f);
		}*/
		glUniform4f(colorizer, 1, 1, 0, 1.0f);

		GLuint modelrotation = glGetUniformLocation(shaderProgram, "model");
		GLuint projection = glGetUniformLocation(shaderProgram, "projection");

		glUniformMatrix4fv(modelrotation, 1, GL_FALSE, value_ptr(model));
		glUniformMatrix4fv(projection, 1, GL_FALSE, value_ptr(project));
		glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_LINE_LOOP, 4 * i, 4);
		glBindVertexArray(0);
		j++;
		i++;
	}
	
	glUseProgram(0);

}
/// <summary>
/// <deprecated>
/// </summary>
void bbox::update() {

	vertices.clear();
	indices.clear();
	unsigned int base = 0;
	for (auto& i : *boxes) {
		int num = 0;

		for (auto& k : i) {
			vertices.insert(vertices.end(), {
				(float)abs(k.y - i[0].y), (float)abs(k.x - i[0].x), 0
				});
			num++;
		}
		int trim = 0;

		indices.insert(indices.end(), {
			base, base + 1, base + 2,
			base + 2, base + 3, base
			});

		base += num;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

