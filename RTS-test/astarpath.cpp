#include "astarpath.h"

using namespace mapinfo;

astarpath::astarpath() {
	map = &arr;
	found = false;
}
astarpath::astarpath(vec2 start, vec2 end) {
	this->start.x = (int)start.x;
	this->start.y = (int)start.y;
	this->end.x = (int)end.x;
	this->end.y = (int)end.y;
	map = &arr;
	found = false;
}
double astarpath::heuristic(int x1, int y1, int x2, int y2) {
	//return pow(x1 - x2, 2) + pow(y1 - y2, 2); //L2
	// return hypot(a.x - b.x, a.y - b.y); //L2
	return abs(x1 - x2) + abs(y1 - y2); //L1 norm
}
void astarpath::calculate() {
	cout << "called" << endl;
	unordered_map<int, Node> nodes;
	priority_queue<Node, vector<Node>, greater<Node>> pq; //openlist

	vector <point> directions{ {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

	nodes[start.x * map[0].size() + start.y] = Node(start, 0, heuristic(start.x, start.y, end.x, end.y)); //closed list
	pq.push(nodes[start.x * map[0].size() + start.y]);
	found = false;

	while (!pq.empty()) {
		Node cur = pq.top(); pq.pop();
		
		if (cur.p == end) { //found
			vector<vec2> path;
			for (Node *node = &cur; node; node = node->parent)
				path.push_back(vec2(node->p.x, node->p.y));
			reverse(path.begin(), path.end());
			results = path;
			found = true;
			return;
		}

		for (auto& dir : directions) {
			int nx = cur.p.x + dir.x, ny = cur.p.y + dir.y;
			if (!isvalid(nx, ny, map)) continue;
			double new_g = cur.g + 1;
			int key = nx * map[0].size() + ny;

			if (!nodes.count(key) || new_g < nodes[key].g) {
				nodes[key] = Node(point(nx, ny), new_g, heuristic(nx, ny, end.x, end.y), &nodes[cur.p.x * map[0].size() + cur.p.y]);
				pq.push(nodes[key]);
			}
 		}
	}
}

bool astarpath::lineofsight(vec2 posa, vec2 posb) { //true 보임 false 벽이랑 충돌
	int x0 = (int)posa.x, y0 = (int)posa.y, x1 = (int)posb.x, y1 = (int)posb.y;
	int dx = abs(x1 - x0), dy = abs(y1 - y0);
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;
	int err = dx - dy;
	int e2;
	int x = x0, y = y0;
	while (true) {
		if (!isvalid(x, y, map)) return false;
		if (x == x1 && y == y1) break;
		e2 = err << 1;
		if (e2 > -dy) { err -= dy; x += sx; }
		if (e2 < dx) { err += dx; y += sy; }
	}
	return true;
}
void astarpath::postsmooth() {
	int k = 0;
	if (results.size() == 0) return;
	vector<vec2> newres;
	newres.push_back(results[0]);
	for (int i = 0; i < results.size() - 1; i++) {
		if (!lineofsight(newres[k], results[i + 1])) {
			k++;
			newres.push_back(results[i]);
		}
	}
	newres.push_back(results[results.size() - 1]);
	results = newres;
}