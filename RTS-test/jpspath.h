#pragma once
#include "pathfinders.h"
class jpspath : public pathfinders
{
public:
	jpspath();
	~jpspath();
	void calculate();
	//void bakemap();
private:
	point jump(point cur, point dir);
	double heuristic(point &a, point &b);
	bool ispassible(point& p);
	inline vector<point> identify_nextpoints(Node * node);
	vector<vector<int>>* map;
};
