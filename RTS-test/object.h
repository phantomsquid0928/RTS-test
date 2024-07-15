#pragma once
#include "headers.hpp"
class object
{
public:
	object();
	object(int x, int y);
	vec4 display2world(vec4 mouse);
	vec4 world2display(vec4 world);
	void changepos(vec2 newpos);
private:

	vec2 position;
};

