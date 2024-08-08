#pragma once
#include "headers.hpp"
#include "pathfinders.h"
#include <map>

struct fieldinfo {
	vector<vector<flownode>> *field;
	/*vector<flownode*> starts;
	vector<bool> visited;*/
	int startscnt;
	int minx, miny, maxx, maxy;
	point end;
	bool ishandled;
	fieldinfo(point end, vector<vector<flownode>> *field = nullptr, int minx = 0, int miny = 0, int maxx = sizex, int maxy = sizey, bool ishandled = false, int startscnt = 0)
		: end(end), minx(minx), miny(miny), maxx(maxx), maxy(maxy), field(field), ishandled(ishandled){};
	fieldinfo()
		: end(point(0, 0)), minx(0), miny(0), maxx(0), maxy(0), field(field), ishandled(false), startscnt(0) {};
	flownode &get(int x, int y) { //get 시에 그대로 넣기
		return (*field)[x - minx][y - miny];
	}
	inline bool isvalid(int x, int y) {
		return x >= minx && y >= miny && x < maxx && y < maxy;
	}
	inline bool isbound(int x, int y) {
		return x > minx && y > miny && x < maxx - 1 && y < maxy - 1;
	}
	
};
class flowfield : public pathfinders
{
public:
	flowfield();
	void calculate();
	vector<vector<flownode>> getfield();
	vec2 getNext(point targetfield, int x, int y);
	vector<vec2> getpath(int x, int y); 
	
private:
	void gencost();
	void genintegrate();
	void genflow();
	
	
	inline bool isvalid(int x, int y, const vector<vector<int>>* map);
	inline bool ispassible(int x, int y, const vector<vector<int>>* map);
	inline double heuristic(int ax, int ay, int bx, int by);

	vector<vector<int>> *map;

	vector<flownode*> qreserve;

	unordered_map<point, fieldinfo, function<size_t(const point&)>, function<bool(const point&, const point&)>> fieldinfos;

	int fieldcnt;
};

