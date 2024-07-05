//#include "block.h"
//
//float vertices[12] = { 0.0f,  0.0f, 0.0f,  // Bottom left
//		 1.0f,  0.0f, 0.0f,  // Bottom right
//		 1.0f,  1.0f, 0.0f,  // Top right
//		 0.0f,  1.0f, 0.0f   // Top left
//};
//
//block::block() { //block default size is 1, position 0, 0
//	vcode = readFile("mouse_vertex.glsl"); //modify by classes
//	fcode = readFile("mouse_fragment.glsl");
//	shaderProgram = createShaderProgram(vcode.c_str(), fcode.c_str());
//	cout << "block shader : " << shaderProgram;
//
//	size = 1;
//
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &IBO);
//
//
//
//
//	unsigned int indices[] = {
//	   0, 1, 2,
//	   2, 3, 0
//	};
//
//	glBindVertexArray(VAO);
//
//
//	// VBO 바인딩 및 데이터 설정
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
//
//	// EBO 바인딩 및 데이터 설정
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//	// 정점 속성 포인터 설정
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//
//	// 바인딩 해제
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//}
//void block::changepos(int posx, int posy) {
//	this->posx = posx;
//	this->posy = posy;
//
//	update();
//}
//void block::render() {
//	// 셰이더 프로그램 사용
//	glUseProgram(shaderProgram);
//	// VAO 바인딩 및 그리기
//	glBindVertexArray(VAO);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//	//glDrawArrays(GL_LINE_LOOP, 0, 4);
//
//	glBindVertexArray(0);
//	glUseProgram(0);
//	//cout << VAO << VBO << IBO;
//}
//
//void block::update() { //made, changepos then update rendering
//
//	// 사각형 크기 업데이트
//	float x1 = (2.0f * posx) / sizex - 1.0f;
//	float y1 = 1.0f - (2.0f * posy) / sizey;
//	float x2 = (2.0f * (posx + size)) / sizex - 1.0f;
//	float y2 = 1.0f - (2.0f * (posy + size)) / sizey;
//
//	vertices[0] = x1; vertices[1] = y1; // Bottom left
//	vertices[3] = x2; vertices[4] = y1; // Bottom right
//	vertices[6] = x2; vertices[7] = y2; // Top right
//	vertices[9] = x1; vertices[10] = y2; // Top left
//
//	//cout << x1 << y1 << "," << x2 << y2 << endl;
//	// VAO, VBO 업데이트
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//
//	//cout << shaderProgram << endl;
//
//
//	
//}
///*mat4 block::getaabb() {
//
//}*/

#include "block.h"

block::block() {
    vcode = readFile("mouse_vertex.glsl");
    fcode = readFile("mouse_fragment.glsl");
    shaderProgram = createShaderProgram(vcode.c_str(), fcode.c_str());
    std::cout << "block shader: " << shaderProgram << std::endl;

    size = 1;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
}

block::~block() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
}

void block::generateBuffers() {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void block::setPositions(const vector<array<int, 2>>& positions) {
    vertices.clear();
    indices.clear();

    for (size_t i = 0; i < positions.size(); ++i) {
        float x1 = (2.0f * positions[i][0]) / sizex - 1.0f;
        float y1 = 1.0f - (2.0f * positions[i][1]) / sizey;
        float x2 = (2.0f * (positions[i][0] + size)) / sizex - 1.0f;
        float y2 = 1.0f - (2.0f * (positions[i][1] + size)) / sizey;

        vertices.insert(vertices.end(), {
            x1, y1, 0.0f,
            x2, y1, 0.0f,
            x2, y2, 0.0f,
            x1, y2, 0.0f
            });

        unsigned int baseIndex = i * 4;
        indices.insert(indices.end(), {
            baseIndex, baseIndex + 1, baseIndex + 2,
            baseIndex + 2, baseIndex + 3, baseIndex
            });
    }

    generateBuffers();
}

void block::render() {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}
