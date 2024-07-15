#pragma once
#include "shader/shader.h"
#include "object.h"

/// <summary>
/// class for bulk-make similar entities at once.
/// </summary>
class path : public shader, object //for 1 tick bulk load changed location, glsubdata with vector <- will make performance better.
{
public:
	path();
	~path();
	void create();
	void render();
	void update(); //will update positon all
	void setpath(vector<vec2> pathlist);
private:
	int size;
	string fcode2;
	vector<float> vertices;
	vector<unsigned int> indices;
	vector<vec2> pathlist;
};