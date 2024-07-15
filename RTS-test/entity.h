#pragma once
#include "shader/shader.h"
#include "object.h"
#include "path.h"

/// <summary>
/// class for bulk-make similar entities at once.
/// </summary>
class entity : public shader, object //for 1 tick bulk load changed location, glsubdata with vector <- will make performance better.
{
public:
	entity();
	entity(const vector<array<float, 3>>& locations);
	~entity();
	void create();
	void render(); 
	void update(float tick); //will update positon all
	vec2 getdir(int offset);
	void setpath(int offset, vector<vec2>* path);
	int getpathidx(int offset);
	int getcurpathidx(int offset);
	vector<vector<vec2>*> getentitypaths();
private:
	int size;
	string fcode2;
	vector<float> vertices;
	vector<unsigned int> indices;
	const vector <array<float, 3>> *location; // x,y, entityno.
	const vector <array<float, 3>>* dest;
	vector<float> rotations;
	vector<vec2> velocity;

	//vector<vec2> location; entitylist can do same feature.

	vector<vector<vec2>*> paths;
	vector<int> curpathidx;
	bitset<100> reached;
	path pathobj;
};

