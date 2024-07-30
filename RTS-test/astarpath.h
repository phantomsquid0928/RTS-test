#pragma once
#include "pathfinders.h"
class astarpath : public pathfinders
{
public:
	astarpath();
	astarpath(vec2 start, vec2 end);
	void calculate(); 
	void postsmooth();
	
private:
	double heuristic(int x1, int y1, int x2, int y2);
	bool lineofsight(vec2 posa, vec2 posb);
	vector<vector<int>> * map;

	inline bool iswall(int x, int y, const vector<vector<int>>* map);
	inline bool ispassible(int x, int y, const vector<vector<int>>* map);
	inline bool isvalid(int x, int y, const vector<vector<int>>* map);

	unordered_map<int, Node> nodes;
	priority_queue<Node, vector<Node>, greater<Node>> pq; //openlist
};

