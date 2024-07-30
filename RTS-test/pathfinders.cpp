#include "pathfinders.h"

pathfinders::pathfinders() {
	this->obj = nullptr;
	found = false;
	last = nullptr;
	usedtime = 0;
}
void pathfinders::calculate() {
	if (obj == nullptr) return;
	//double starttime = glfwGetTime();

	chrono::system_clock::time_point start;
	chrono::system_clock::time_point end;
	start = chrono::system_clock::now();
	obj->calculate();
	end = chrono::system_clock::now();
	typedef chrono::duration<double, std::milli> millisec;
	//cout << to_string(chrono::duration_cast<millisec> (end - start).count()) << endl;

	//double endtime = glfwGetTime();
	usedtime = chrono::duration_cast<millisec> (end - start).count();
	results = obj->results;
}
void pathfinders::setfunc(pathfinders * obj) {
	this->obj = obj;
}
//inline bool pathfinders::iswall(int x, int y, const vector<vector<int>>* map) {
//	return !isvalid(x, y, map) || (*map)[x][y] == 1;
//}
//inline bool pathfinders::ispassible(int x, int y, const vector<vector<int>>* map) { //TODO : map is mapinfo :: arr
//	return isvalid(x, y, map) && (*map)[x][y] != 1;
//}
//inline bool pathfinders::isvalid(int x, int y, const vector<vector<int>>* map) { //TODO : map is mapinfo :: arr
//	return x >= 0 && y >= 0 && x < sizey && y < sizex;
//}
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