#pragma once
#include "headers.hpp"
struct point {
	int x, y;
	//int clusternum = -1;
	point(int x, int y) : x(x), y(y) {}
	point() : x(0), y(0) {}

	bool operator==(point& other) const {
		return x == other.x && y == other.y;
	}
	bool operator !=(point& other) const {
		return x != other.x || y != other.y;
	}

	bool operator <(point& other) const {
		return x < other.x || (x == other.x && y < other.y);
	}
	//bool operator -(point& other) const {
	//	return clusternum - other.clusternum;
	//}
};
struct Node {
	point p;
	double g, h;
	Node* parent;
	int status;

	Node(point p, int status = 0, double g = 0, double h = 0, Node* parent = nullptr) : p(p), g(g), h(h), parent(parent), status(status) {}
	Node() : p(point()), g(0), h(0), parent(nullptr), status(0){};
	double f() const {
		return g + h;
	}

	bool operator < (const Node& other) const {
		return f() > other.f();
	}
	bool operator > (const Node& other) const {
		return f() > other.f();
	}
};
struct flownode {
	int x, y;
	point bestdir;
	bool isstart;
	int cost;
	int bestcost;
	bool visited;
	flownode(int x, int y, point bestdir = point(0, 0), int cost = 1, int bestcost = INT_MAX, bool isstart = false, bool visited = false)
		: x(x), y(y), bestdir(bestdir), cost(cost), bestcost(bestcost), isstart(false), visited(false)
	{};
	flownode() : x(0), y(0), bestdir(point(0, 0)), cost(1), bestcost(INT_MAX), isstart(false), visited(false) {};
};
class pathfinders
{
public:
	pathfinders();
	void setfunc(pathfinders * obj);
	virtual void calculate();
	vector<vec2> getResult();
	void setstart(vec2 pos);
	void setend(vec2 pos);
	double getusedtime();
	void removefirst();
protected:
	point start;
	point end;
	vector<vec2> results;
	Node* last;
	//vector<point> directions = { {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1} };
	//							//down , right , up   ,  left    ,  downr, upleft  , downl ,  upright

	vector<point> directions = { {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1} }; //anti clockwise
	bool found;
	bool isvalid(int x, int y, const vector<vector<int>>* map);
	bool ispassible(int x, int y, const vector<vector<int>>* map);
	bool iswall(int x, int y, const vector<vector<int>>* map);
	double usedtime;
	pathfinders* obj;
};

