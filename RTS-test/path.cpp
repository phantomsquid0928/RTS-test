#include "path.h"
#include "headers.hpp"

using namespace input;
using namespace mapinfo;
//line юс
//latest version of rendering objects
path::path() {
	vcode = readFile("shader/entity_vertex.glsl"); //mod
	fcode = readFile("shader/entity_fragment.glsl");

	shaderProgram = createShaderProgram(vcode.c_str(), fcode.c_str());

	cout << "entity shader : " << shaderProgram << endl;

	size = 1;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
}

path::~path() {
}
void path::setpath(vector<vec2> pathlist) {
	this->pathlist = pathlist;
}
void path::create() { //TODO : refactoring of vertices, indices needed
	//generate buffers
	vertices.clear();
	indices.clear();

	float x1 = -(float)size / 2;
	float x2 = (float)size / 2;
	float y1 = -(float)size / 2;
	float y2 = (float)size / 2;

	vertices.insert(vertices.end(), {
			x1, y1, 0.0f,   // -1, -1
			x2, y1, 0.0f,    // 1, -1
			x2, y2, 0.0f,    // 1, 1
			x1, y2, 0.0f    //  -1, 1
		});

	unsigned int base = 0;
	indices.insert(indices.end(), {
		base, base + 1, base + 2,
		base + 2, base + 3, base
		});


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
void path::render() { //TODO: need to assume shaderProgram called.
	//glUseProgram(shaderProgram);	
	//glDrawArrays(GL_LINE_LOOP, 0, 4);

	glUseProgram(shaderProgram);
	int i = 0;
	vec2 old;
	old = pathlist[0];
	for (auto p : pathlist) {
		vec2 diff = p - old;
		float lensq = pow(diff.x, 2) + pow(diff.y, 2);
		if (lensq > 1) {
			int loopsize = sqrt(lensq);
			
			vec2 a = vec2(old);
			vec2 add = normalize(diff);
			
			for (int k = 1; k <= loopsize; k++) {
				a = a + add;

				mat4 model = mat4(1.0f);
				mat4 project = mat4(1.0f);


				//model = translate(model, vec3(2 * entitylist[i][0] / sizex - 1, 1 - 2 * entitylist[i][1] / sizey, 0));
				model = translate(model, vec3(a, 0));

				project = ortho(0.0f, (float)sizex, (float)sizey, 0.0f, -1.0f, 1.0f);

				GLuint colorizer = glGetUniformLocation(shaderProgram, "color");
				/*if (selected[i]) {
					glUniform4f(colorizer, 0.0f, 1.0f, 0.0f, 1.0f);
				}
				else {
					glUniform4f(colorizer, 0.0f, 0.0f, 1.0f, 1.0f);
				}*/
				glUniform4f(colorizer, 0.0f, 1.0f, 0.0f, 1.0f);

				GLuint modelrotation = glGetUniformLocation(shaderProgram, "model");
				GLuint projection = glGetUniformLocation(shaderProgram, "projection");

				glUniformMatrix4fv(modelrotation, 1, GL_FALSE, value_ptr(model));
				glUniformMatrix4fv(projection, 1, GL_FALSE, value_ptr(project));



				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
				//glDrawArrays(GL_LINE_LOOP, 0, indices.size());
				glBindVertexArray(0);

				i++;
			}
		}
		old = p;
		mat4 model = mat4(1.0f);
		mat4 project = mat4(1.0f);


		//model = translate(model, vec3(2 * entitylist[i][0] / sizex - 1, 1 - 2 * entitylist[i][1] / sizey, 0));
		model = translate(model, vec3(p, 0));

		project = ortho(0.0f, (float)sizex, (float)sizey, 0.0f, -1.0f, 1.0f);

		GLuint colorizer = glGetUniformLocation(shaderProgram, "color");
		/*if (selected[i]) {
			glUniform4f(colorizer, 0.0f, 1.0f, 0.0f, 1.0f);
		}
		else {
			glUniform4f(colorizer, 0.0f, 0.0f, 1.0f, 1.0f);
		}*/
		glUniform4f(colorizer, 0.0f, 1.0f, 0.0f, 1.0f);

		GLuint modelrotation = glGetUniformLocation(shaderProgram, "model");
		GLuint projection = glGetUniformLocation(shaderProgram, "projection");

		glUniformMatrix4fv(modelrotation, 1, GL_FALSE, value_ptr(model));
		glUniformMatrix4fv(projection, 1, GL_FALSE, value_ptr(project));



		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_LINE_LOOP, 0, indices.size());
		glBindVertexArray(0);
		
		i++;
	}

	glUseProgram(0);

}
/// <summary>
/// <deprecated>
/// </summary>
void path::update() { //will be called per tick?

}