#pragma once

#include "headers.hpp"
#include "shader/shader.h"

class wall : public shader
{
public:
	wall();
	void render();
	//void update();
	//void delta(int x, int y);
	mat4 getaabb();
	void update();
	void create();
private:
	int size;
	vector<float> vertices;
	vector<unsigned int> indices;
};

