#pragma once
#include "headers.hpp"
struct point {
	int x, y;
	point(int x, int y) : x(x), y(y) {}
	point() : x(0), y(0) {}

	bool operator==(point& other) const {
		return x == other.x && y == other.y;
	}
	bool operator !=(point& other) const {
		return x != other.x || y != other.y;
	}
};
struct Node {
	point p;
	double g, h;
	Node* parent;

	Node(point p, double g = 0, double h = 0, Node* parent = nullptr) : p(p), g(g), h(h), parent(parent) {}
	Node() : p(point()), g(0), h(0), parent(nullptr) {};
	double f() const {
		return g + h;
	}

	bool operator > (const Node& other) const {
		return f() > other.f();
	}
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
	bool found;
	bool isvalid(int x, int y, const vector<vector<int>>* map);
	double usedtime;
private:
	pathfinders* obj;
};
