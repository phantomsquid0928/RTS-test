#include "pathfinders.h"

pathfinders::pathfinders() {
	this->obj = nullptr;
	found = false;
	usedtime = 0;
}
void pathfinders::calculate() {
	if (obj == nullptr) return;
	double starttime = glfwGetTime();
	obj->calculate();
	double endtime = glfwGetTime();
	usedtime = endtime - starttime;
	results = obj->results;
}
void pathfinders::setfunc(pathfinders * obj) {
	this->obj = obj;
}
bool pathfinders::isvalid(int x, int y, const vector<vector<int>>* map) { //TODO : map is mapinfo :: arr
	return x >= 0 && y >= 0 && x < map->size() && y < map->at(0).size() && (*map)[x][y] != 1;
}
double pathfinders::getusedtime() {
	return usedtime;
}
vector<vec2> pathfinders::getResult() {
	return obj->found == true ? obj->results : vector<vec2>(0); 
}
void pathfinders::setstart(vec2 pos) {
	obj->start.x = (int)pos.x;
	obj->start.y = (int)pos.y;

}
void pathfinders::setend(vec2 pos) {
	obj->end.x = (int)pos.x;
	obj->end.y = (int)pos.y;
}
void pathfinders::removefirst() {
	if (obj->results.empty()) return;
	obj->results.erase(obj->results.begin());
}