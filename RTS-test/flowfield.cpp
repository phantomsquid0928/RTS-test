#include "flowfield.h"

using namespace mapinfo;
flowfield::flowfield() {
	map = &mapinfo::arr;
	//qreserve.reserve(1000000);


	fieldcnt = -1;
	function<size_t(const point&)>  hashfunc = [](const point& p) {
		return hash<int>{}(p.x) ^ hash<int>{}(p.y);
	};
	function<bool(const point&, const point&)>  equals = [](const point& p, const point& p1) -> bool {
		return p.x == p1.x && p.y == p1.y;
	};
	fieldinfos = unordered_map<point, fieldinfo, decltype(hashfunc), decltype(equals)>{ 10, hashfunc, equals };
}
inline bool flowfield::isvalid(int x, int y, const vector<vector<int>>* map) { //TODO : map is mapinfo :: arr
	return x >= 0 && y >= 0 && x < sizex&& y < sizey;
}
inline bool flowfield::ispassible(int x, int y, const vector<vector<int>>* map) { //TODO : map is mapinfo :: arr
	return isvalid(x, y, map) && ((*map)[x][y] != 1 && (*map)[x][y] != 2);
}

void flowfield::calculate() { //call once and make path for entities.
	/*vector<point> erasetargets;
	bool changed = false;
	int vacancy = -1;
	for (auto& t : pointmapper) {
		int found = false;
		for (auto& [x, y, num] : destlist) {
			if ((int)x == t.first.x && (int)y == t.first.y) {
				found = true;
				break;
			}
		}
		if (!found) {
			vacancy = t.second;
			changed = true;
			pointmapper.erase(t.first);
		}
	}

	if (vacancy == -1) {
		pointmapper[end] = ++fieldcnt;
	}
	else {
		changed = true;
		pointmapper[end] = vacancy;
	}
	*/
	bool changed = false;

	int minx = end.x;
	int miny = end.y;
	int maxx = end.x;
	int maxy = end.y;


	for (auto& [x, y, i] : entitylist) {
		if (selected[(int)i]) {
			minx = std::min((int)x, minx);
			miny = std::min((int)y, miny);
			maxx = std::max((int)x, maxx);
			maxy = std::max((int)y, maxy);
		}
	}
	
	int size = 50;
	minx = std::max(0, minx - size);
	miny = std::max(0, miny - size);
	maxx = std::min(maxx + size, sizex);
	maxy = std::min(maxy + size, sizey);
	int boundx = std::min(maxx - minx, sizex);
	int boundy = std::min(maxy - miny, sizey);
	cout << minx <<":" << miny << ":" << maxx << ":" << maxy << endl;
	auto nodes = new vector<vector<flownode>>(boundx, vector<flownode>(boundy, flownode())); //harsh?
	//auto nodes = new vector<vector<flownode>>(sizex, vector<flownode>(sizey, flownode())); //harsh?
	fieldinfo info = fieldinfo(end, nodes, minx, miny, maxx, maxy);
	//fieldinfo info = fieldinfo(end, nodes, 0, 0, sizex, sizey);

	int cnt = 0;
	for (auto& [x, y, i] : destlist) {
		found = false;
		if (selected[i]) {
			info.get((int)entitylist[i][0], (int)entitylist[i][1]).isstart++;
			cnt++;
		}
		if (fieldinfos.count(point(x, y))) {
			fieldinfos[point(x, y)].ishandled = true;
		}
	}
	info.startscnt = cnt;
	auto it = fieldinfos.begin();
	while (it != fieldinfos.end()) {
		if ((*it).second.ishandled == false) {
			delete fieldinfos[(*it).first].field;
			it = fieldinfos.erase(it);
			continue;
		}
		(*it).second.ishandled = false;
		it++;
	}
	/*for (auto& k : fieldinfos) { //if this thing makes suck then change outer code of flowfield to .
		found = false;
		for (auto& [x, y, i] : destlist) {
			if (k.first.x == x && k.first.y == y) {
				found = true;
				break;
			}
		}
		if (!found) {
			delete fieldinfos[k.first].field;
			fieldinfos.erase(k.first);
		}
	}*/

	fieldinfos[end] = info;
	gencost();
	genintegrate();
	genflow();

	/*for (int i = minx; i < minx + 20; i++) {
		for (int j = miny; j < miny + 20; j++) {
			cout << fieldinfos[end].get(i, j).bestdir.x << "," << fieldinfos[end].get(i, j).bestdir.y << " ";
		}
		cout << endl;
	}*/
}
inline double flowfield::heuristic(int ax, int ay, int bx, int by) {
	//return abs(a.x - b.x) + abs(a.y - b.y);
	return hypot(ax - bx, ay - by);
	//return std::max(abs(a.x - b.x), abs(a.y - b.y)) + 1.41 * std::min(abs(a.x - b.x), abs(a.y - b.y)); //octile
}
// github.com/JohnnyTurbo
void flowfield::gencost() { //is time complexity okay???? ispassible can make this thing useless?

	fieldinfo& f = fieldinfos[end];
	for (int i = f.minx; i < f.maxx; i++) { //same coord
		for (int j = f.miny; j < f.maxy; j++) { //same coord
			f.get(i, j).x = i;
			f.get(i, j).y = j;
			if ((*map)[i][j] == 1 || (*map)[i][j] == 2) //wall
			{
				f.get(i, j).bestdir.x = 0;
				f.get(i, j).bestdir.y = 0;
				f.get(i, j).cost = INT_MAX;
				f.get(i, j).bestcost = INT_MAX;
				continue;
			}
			f.get(i, j).bestdir.x = 0;
			f.get(i, j).bestdir.y = 0;
			f.get(i, j).cost = 1;
			f.get(i, j).bestcost = INT_MAX;
		}
	}
}

void flowfield::genintegrate() { //is time complexity okay???? seems dijkstra...

	queue<point> q;
	queue<point> candidate;
	int candidatedir[8] = {0, }; // directions as candidates? ok
	
	fieldinfo& f = fieldinfos[end];
	f.get(end.x, end.y).bestcost = 0;

	q.emplace(end);
	
	//while( all start r visited )  <- how to check it?
	// do expend
	// after expend if not visited then get candidates, push then, embark vector towards the candidates direction:
	// embark : 
	//			if dir 6 exists then
	//			if (miny - size < 0) then size = miny;
	//	
	//			for i in field i.pushfront new flownode for size
	//			miny -= size;
	//			if dir 2 exists then
	//			if (maxy + size > sizex) then size = sizex - maxy
	//			for i in field i.pushfront new flownode for size
	//			
	//			for i in field
	//				i.push_front new flownode for size
	//			for i in range 0, size
	//				i.push_front new vector<flownode>(size + maxx - minx, flownode());
	//while()
	int visited = 0;
	//bool lv = false;
	bool afterembark = false;

	while (true) {
		
		while (!q.empty()) {
			
			//if (afterembark) cout << cur.x << ":" << cur.y << endl;
			if (q.front().x == 0 && q.front().y == 0) {
				q.pop();
				continue;
			}
			flownode &cur = f.get(q.front().x, q.front().y);
			q.pop();
			if (cur.visited) continue;
			cur.visited = true;
			
			for (int dir = 0; dir < directions.size(); dir++) {
				point d = directions[dir];
				//if (!ispassible(cur.x + d.x, cur.y + d.y, map)) continue; //cheat
				if (!f.isvalid(cur.x + d.x, cur.y + d.y)) continue;
				flownode& neighbor = f.get(cur.x + d.x, cur.y + d.y);
				if (neighbor.x != cur.x + d.x) {
					//cout << "called";
					neighbor.x = cur.x + d.x;
					neighbor.y = cur.y + d.y;
					neighbor.cost = (*map)[cur.x + d.x][cur.y + d.y] != 0 ? INT_MAX : 1; //maybe useless cuz of ispassible
				}
		
				//flownode* neighbor = &field[cur->x + d.x][cur->y + d.y];
				//cout << cur->x + d.x << " : " << cur->y + d.y << endl;
				

				//if neighbor was embarked so info was omitted.
				if (neighbor.isstart) {
					neighbor.isstart--;
					visited++;
				}
				if (neighbor.cost == INT_MAX) continue; //useless when uses map as cheat sheet
				int nextcost = neighbor.cost + cur.bestcost + heuristic(cur.x, cur.y, end.x, end.y);
				if (nextcost < neighbor.bestcost) {
					neighbor.bestcost = neighbor.cost + cur.bestcost + heuristic(cur.x, cur.y, end.x, end.y);
					/*if (afterembark) {
						cout << "neighbor is : " << neighbor.x << " : " << neighbor.y << endl;
					}*/
					if (!f.isbound(cur.x + d.x, cur.y + d.y) && !neighbor.visited) {
						//if (neighbor.lvflip != lv) continue; //;?
						//if (afterembark) cout << "reached end" << cur.x + d.x << ":" << cur.x + d.y << endl;

						candidate.emplace(point(neighbor.x, neighbor.y));
						candidatedir[dir] = 1;
						continue;
					}
					q.emplace(point(neighbor.x, neighbor.y));
				}
				
			}
		}
		if (visited >= f.startscnt) {
			cout << "found!!" << endl;
			break;
		}
		/*if (firstminx >= f.minx && firstminy >= f.miny && firstmaxx <= f.maxx && firstmaxy <= f.maxy) {
			break;
		}*/
		if (candidate.empty()) {
			cout << "candidate empty" << endl;
			break; //no path... for some target?
		}
		q.swap(candidate); //awesome

		
		//lv ^= true;
		afterembark = true;
		//embark flownode area
		cout << "trying embark" << endl;

		if (candidatedir[5] || candidatedir[6] || candidatedir[7]) { //left embark
			int size = 100;
			size = f.miny - size < 0 ? f.miny : size;
			f.miny = f.miny - size;

			for (auto& r : *f.field) {
				vector<flownode> temp = vector<flownode>(size, flownode());
				r.insert(r.begin(), temp.begin(), temp.end());
			}
		}

		if (candidatedir[1] || candidatedir[2] || candidatedir[3]) { //right embark
			int size = 100;
			size = f.maxy + size >= sizey ? sizey - f.maxy : size;
			f.maxy = f.maxy + size;

			for (auto& r : *f.field) {
				vector<flownode> temp = vector<flownode>(size, flownode());
				r.insert(r.end(), temp.begin(), temp.end());
			}
		}

		if (candidatedir[7] || candidatedir[0] || candidatedir[1]) { //down embark
			int size = 100;
			size = f.maxx + size >= sizex ? sizex - f.maxx : size;
			f.maxx = f.maxx + size;


			for (int t = 0; t < size; t++) (*f.field).push_back(vector<flownode>(f.maxy - f.miny, flownode()));
			
		}

		if (candidatedir[3] || candidatedir[4] || candidatedir[5]) { //up embark
			int size = 100;
			size = f.minx - size < 0 ? f.minx : size;
			f.minx = f.minx - size;

			for (int t = 0; t < size; t++) (*f.field).insert((*f.field).begin(), vector<flownode>(f.maxy - f.miny, flownode()));
		}

		cout << "embarked to " << f.minx << f.miny << f.maxx << f.maxy << endl;
		memset(candidatedir, 0, sizeof(candidatedir));
	}
	
	/*q.emplace(&field[end.x][end.y]);
	while (!q.empty()) {
		flownode* cur = q.front();
		q.pop();

		for (point &d : directions) {
			if (!isvalid(cur->x + d.x, cur->y + d.y, map)) continue;
			flownode* neighbor = &field[cur->x + d.x][cur->y + d.y];
			if (neighbor->cost == INT_MAX) continue;
			int nextcost = neighbor->cost + cur->bestcost + heuristic(cur->x, cur->y, end.x, end.y);
			if (nextcost < neighbor->bestcost) {
				neighbor->bestcost = neighbor->cost + cur->bestcost + heuristic(cur->x, cur->y, end.x, end.y);
				q.emplace(neighbor);
			}
		}
	}*/
}

void flowfield::genflow() {
	fieldinfo& f = fieldinfos[end];
	auto field = *f.field;
	for (int i = f.minx; i < f.maxx; i++) {
		for (int j = f.miny; j < f.maxy; j++) {
			int bestcost = f.get(i, j).bestcost;
			point bestdir = point(0, 0);
			for (point& d : directions) {
				if (!f.isvalid(i + d.x, j + d.y)) continue;
				if (!ispassible(i + d.x, j + d.y, map)) continue; //wall
				flownode& neighbor = f.get(i + d.x, j + d.y);// field[j.x + d.x][j.y + d.y];
				if (neighbor.cost == INT_MAX) continue; //wall
				if (neighbor.bestcost < bestcost) {
					bestcost = neighbor.bestcost;
					bestdir.x = neighbor.x - f.get(i, j).x;
					bestdir.y = neighbor.y - f.get(i, j).y;
				}
			}

			f.get(i, j).bestdir = bestdir;
		}
	}
}
/// <summary>
/// deprecated
/// </summary>
/// <returns></returns>
vector<vector<flownode>> flowfield::getfield() { 
	return vector<vector<flownode>>();
}

vec2 flowfield::getNext(point targetfield, int x, int y) {
	if (!isvalid(x, y, map)) {
		cout << x << ":" << y << endl;
		throw this;
	}
	if (!fieldinfos.count(targetfield)) {
		return vec2(0, 0);
	}
	auto f = fieldinfos[targetfield];
	if (!f.isvalid(x, y)) {
		cout << "on wrong field" << endl;
		return vec2(0, 0);
	}
	return vec2(f.get(x, y).bestdir.x, f.get(x, y).bestdir.y);
}

/// <summary>
/// only works correctly after call calculate cuz of 'end'
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
vector<vec2> flowfield::getpath(int x = -1, int y = -1) {//setstart then get specific entity's path
	if (x == -1) {
		x = start.x;
		y = start.y;
	}
	vector<vec2> path;
	point cur = point(x, y);
	//path.emplace_back(vec2(cur.x, cur.y));

	auto f = fieldinfos[end];
	//cout << "CALLED" << endl;
	while (true) {
		if (cur == end) break;
		//cout << cur.x << cur.y << endl;
		point& dir = f.get(cur.x, cur.y).bestdir;//[cur.x][cur.y].bestdir;
		if (dir.x == 0 && dir.y == 0) {
			//cout << "broken" << endl;
			return vector<vec2>(0);
		}
		vec2 next = vec2(cur.x + dir.x, cur.y + dir.y);
		path.emplace_back(next);
		cur.x = (int)next.x;
		cur.y = (int)next.y;
	}
	return path;
}