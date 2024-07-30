#pragma once

#include "headers.hpp"
#include "../shader/shader.h"

class block : public shader
{
public:
	block();
	block(const vector<array<int, 2>>& positions);
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
	const vector<array<int, 2>>* positions;
};

