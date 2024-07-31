#pragma once
#include "headers.hpp"
#include "pathfinders.h"

class flowfield : public pathfinders
{
public:
	flowfield();
	void calculate();
	vector<vector<flownode>> getfield();
private:
	void gencost();
	void genintegrate();
	void genflow();
	
	inline bool isvalid(int x, int y, const vector<vector<int>>* map);

	vector<vector<int>> *map;
	vector<vector<flownode>> field;
	vector<flownode*> qreserve;
};

