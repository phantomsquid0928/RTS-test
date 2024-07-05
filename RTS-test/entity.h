#pragma once
#include "object.h"
#include "shader/shader.h"

class entity : public shader, object
{
public:
	entity();
	void render();
	void update();
	
};

