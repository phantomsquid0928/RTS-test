#include "jpspath.h"

using namespace mapinfo;

jpspath::jpspath() {
	map = &arr;
	found = false;
	//TODO: need precompute, and change to other jps that faster than astar
}

double jpspath::heuristic(point& a, point& b) {
	return abs(a.x - b.x) + abs(a.y - b.y);
	//return hypot(a.x - b.x, a.y - b.y);
}
point jpspath::jump(point cur, point dir) {
	//point next = cur;
	//while (true) {
	//	next.x += dir.x;
	//	next.x += dir.y;

	//	if (!isvalid(next.x, next.y, map))
	//		return point{ -1, -1 };

	//	if (next == end)
	//		return next;
	//	if ((dir.x != 0 && (isvalid(next.x + dir.x, next.y, map) && !isvalid(next.x + dir.x, next.y - 1, map)))
	//		|| (dir.y != 0 && (isvalid(next.x, next.y + dir.y, map) && !isvalid(next.x - 1, next.y + dir.y, map)))) {
	//		return next;
	//	}
	//	if ((dir.x != 0 && (isvalid(next.x + dir.x, next.y, map) && !isvalid(next.x + dir.x, next.y - 1, map)))
	//		|| (dir.y != 0 && (isvalid(next.x, next.y + dir.y, map) && !isvalid(next.x - 1, next.y + dir.y, map)))) {
	//		return next;
	//	}
	//	if ((dir.x != 0 && (isvalid(next.x + dir.x, next.y, map) && !isvalid(next.x + dir.x, next.y - 1, map)))
	//		|| (dir.y != 0 && (isvalid(next.x, next.y + dir.y, map) && !isvalid(next.x - 1, next.y + dir.y, map)))) {
	//		return next;
	//	}
	//	if ((dir.x != 0 && (isvalid(next.x + dir.x, next.y, map) && !isvalid(next.x + dir.x, next.y - 1, map)))
	//		|| (dir.y != 0 && (isvalid(next.x, next.y + dir.y, map) && !isvalid(next.x - 1, next.y + dir.y, map)))) {
	//		return next;
	//	}
	//}
	point next(cur.x + dir.x, cur.y + dir.y);

	if (!isvalid(next.x, next.y, map)) return point{ -1, -1 };
	if (next == end)
		return next;

	if ((dir.x != 0 && (isvalid(next.x + dir.x, next.y, map) && !isvalid(next.x + dir.x, next.y - 1, map)))
		|| (dir.y != 0 && (isvalid(next.x, next.y + dir.y, map) && !isvalid(next.x - 1, next.y + dir.y, map)))) {
		return next;
	}

	if (dir.x != 0 && dir.y != 0) {
		point jumpx = jump(point(next.x, next.y), point(dir.x, 0));
		point jumpy = jump(point(next.x, next.y), point(0, dir.y));

		if (jumpx.x != -1 || jumpy.y != -1) {
			return next;
		}
	}

	return jump(next, dir);
}
inline vector<point> jpspath::identify_nextpoints(Node* node) {
	vector<point> nexts;
	vector<point> directions = { {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1} };

	for (auto& dir : directions) {
		point jumppoint = jump(node->p, dir);
		if (jumppoint.x != -1) {
			nexts.push_back(jumppoint);
		}
	}
	return nexts;
}

void jpspath::calculate() {
	unordered_map<int, Node> nodes; //closedlist
	priority_queue < Node, vector<Node>, greater<Node>> pq; //openlist
	auto hashfunc = [](point& p) {
		return p.x * 10000 + p.y;
	};
	auto getNode = [&nodes, &hashfunc](point& p) -> Node& { return nodes[hashfunc(p)]; };

	nodes[hashfunc(start)] = Node(start, 0, heuristic(start, end));
	pq.push(nodes[hashfunc(start)]);

	while (!pq.empty()) {
		Node cur = pq.top(); pq.pop();

		if (cur.p == end) {
			vector<vec2> path;
			for (Node* node = &cur; node; node = node->parent)
				path.push_back(vec2(node->p.x, node->p.y));

			reverse(path.begin(), path.end());
			results = path;
			found = true;
			return;
		}
		
		for (auto& next : identify_nextpoints(&cur)) {
			double newg = cur.g + heuristic(cur.p, next);

			if (!nodes.count(hashfunc(next)) || newg < getNode(next).g) {
				nodes[hashfunc(next)] = Node(next, newg, heuristic(next, end), &getNode(cur.p));
				pq.push(nodes[hashfunc(next)]);
			}
		}
	}
}
