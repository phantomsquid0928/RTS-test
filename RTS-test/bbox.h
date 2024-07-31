#pragma once
#include "shader/shader.h"
#include "object.h"
#include "pathfinders.h"

/// <summary>
/// class for bulk-make similar entities at once.
/// </summary>
class bbox: public shader, object //for 1 tick bulk load changed location, glsubdata with vector <- will make performance better.
{
public:
	bbox();
	bbox(vector<vector<point>>* boxes);
	~bbox();
	void create();
	void render();
	void update();
private:
	int size;
	string fcode2;
	vector<float> vertices;
	vector<unsigned int> indices;
	vector<vector<point>> * boxes;
};

