#include "flowfield.h"

flowfield::flowfield() {
	map = &mapinfo::arr;
	//qreserve.reserve(1000000);
	field.reserve(10000000);
	field = vector<vector<flownode>>(sizex, vector<flownode>(sizey, flownode()));
}
inline bool flowfield::isvalid(int x, int y, const vector<vector<int>>* map) { //TODO : map is mapinfo :: arr
	return x >= 0 && y >= 0 && x < sizex&& y < sizey;
}

void flowfield::calculate() { //call once and make path for entities.
	gencost();
	genintegrate();
	genflow();
}
// github.com/JohnnyTurbo
void flowfield::gencost() { //is time complexity okay????
	for (int i = 0; i < sizex; i++) {
		for (int j = 0; j < sizey; j++) {
			field[i][j].x = i;
			field[i][j].y = j;
			if ((*map)[i][j] == 1) //wall
			{
				field[i][j].bestdir.x = 0;
				field[i][j].bestdir.y = 0;
				field[i][j].cost = INT_MAX;
				field[i][j].bestcost = INT_MAX;
				continue;
			}
			field[i][j].bestdir.x = 0;
			field[i][j].bestdir.y = 0;
			field[i][j].cost = 1;
			field[i][j].bestcost = INT_MAX;
		}
	}
	
}

void flowfield::genintegrate() { //is time complexity okay???? seems dijkstra...
	field[end.x][end.y].bestcost = 0;
	queue<flownode*> q;
	
	q.emplace(&field[end.x][end.y]);
	while (!q.empty()) {
		flownode* cur = q.front(); 
		q.pop();
		
		for (point &d : directions) {
			if (!isvalid(cur->x + d.x, cur->y + d.y, map)) continue;
			flownode* neighbor = &field[cur->x + d.x][cur->y + d.y];
			if (neighbor->cost == INT_MAX) continue;
			if (neighbor->cost + cur->bestcost < neighbor->bestcost) {
				neighbor->bestcost = neighbor->cost + cur->bestcost;
				q.emplace(neighbor);
			}
		}
	}
}

void flowfield::genflow() {
	for (auto& i : field) {
		for (auto& j : i) {
			int bestcost = j.bestcost;
			point bestdir = point(0, 0);
			for (point& d : directions) {
				if (!isvalid(j.x + d.x, j.y + d.y, map)) continue;
				flownode& neighbor = field[j.x + d.x][j.y + d.y];
				if (neighbor.bestcost < bestcost) {
					bestcost = neighbor.bestcost;
					bestdir.x = neighbor.x - j.x;
					bestdir.y = neighbor.y - j.y;
				}
			}

			j.bestdir = bestdir;
		}
	}
}

vector<vector<flownode>> flowfield::getfield() {
	return field;
}