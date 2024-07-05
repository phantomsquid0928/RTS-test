//#pragma once
//
//#include "headers.hpp"
//#include "../shader/shader.h"
//#include "object.h"
//
//class block : public shader, object
//{
//public:
//	block();
//	void render();
//	void update();
//	//void delta(int x, int y);
//	mat4 getaabb();
//	void changepos(int posx, int posy);
//private:
//	int size;
//	int posx, posy;
//};
//

#ifndef BLOCK_H
#define BLOCK_H

#include "headers.hpp"
#include "../shader/shader.h"
#include "object.h"

class block : public shader, object{
public:
    block();
    ~block();
    void setPositions(const std::vector<std::array<int, 2>>& positions);
    void render();

private:
    GLuint VAO, VBO, IBO;
    GLuint shaderProgram;
    int size;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void generateBuffers();
};

#endif // BLOCK_H

