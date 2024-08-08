#include "jpspath.h"

using namespace mapinfo;

jpspath::jpspath() {
	map = &arr;
	found = false;
	obj = this;
	
	//TODO: need precompute, and change to other jps that faster than astar
	pqreserve.reserve(100000);
	nodes.reserve(10000000);
	nexts.reserve(100);
	table.reserve(100000);
	clusters.reserve(100000);
	convexhulls.reserve(100000);
	
	cboundtable.reserve(100000);
	//memset(table, 0, sizeof(table));
	//table.resize(sizey, vector<vector<int>>(sizex, vector<int>(9, 0)));

	//table = vector<vector<vector<int>>>(sizex, vector<vector<int>>(sizey, vector<int>(9, 0)));
	usingbounding = true;
	//openlist = vector<vector<Node>>(sizex, vector<Node>(sizey, Node()));
}

void jpspath::changemod(bool usingbounding) {
	if (this->usingbounding == usingbounding) return;
	this->usingbounding = usingbounding;
}
void jpspath::loadTable() {
	ifstream ifs;
	
	ifs.open("jpbakedmap.dat");
	if (!ifs.is_open()) {
		return;
	}
	table = vector<vector<vector<int>>>(sizex, vector<vector<int>>(sizey, vector<int>(10, 0)));
	jps.clear();
	string input = "";
	string inner = "";
	
	

	int idx = 0;
	int idy = 0;
	while (!ifs.eof()) {
		getline(ifs, input);
		istringstream issout;
		issout.str(input);
		while (getline(issout, inner, '|')) {
			//cout << "inner : " << inner << endl;
			string data;
			istringstream iss;
			iss.str(inner);
			int d = 0;
			while (getline(iss, data, ',')) {
				//table[idx][idy][d] = stoi(data);

				//cout << idx << "," << idy << "," << d << ":" << stoi(data) << endl;
				table[idx][idy][d] = stoi(data);
				if (table[idx][idy][d] == -1 && d == 8) jps.push_back(point(idx, idy));
				d++;
			}
			//cout << endl;
			idy++;
		}
		if (idy == sizey) {
			idx++;
			idy = 0;
		}
	}

	ifs.close();

	if (usingbounding) boundingcalc();
}
void jpspath::saveTable() {
	ofstream ofs;

	ofs.open("jpbakedmap.dat", ios::out | ios::trunc);

	for (int i = 0; i < sizex; i++) {
		for (int j = 0; j < sizey; j++) {
			for (int d = 0; d < 9; d++) {
				ofs << table[i][j][d] << ",";
			}
			ofs << "|";
		}
		ofs << endl;
	}
	ofs.close();
}

inline bool jpspath::iswall(int x, int y, const vector<vector<int>>* map) {
	return !isvalid(x, y, map) || ((*map)[x][y] == 1 || (*map)[x][y] == 2); //2 is for size padding
}
inline bool jpspath::ispassible(int x, int y, const vector<vector<int>>* map) { //TODO : map is mapinfo :: arr
	return isvalid(x, y, map) && ((*map)[x][y] != 1 && (*map)[x][y] != 2);
}
inline bool jpspath::isvalid(int x, int y, const vector<vector<int>>* map) { //TODO : map is mapinfo :: arr
	return x >= 0 && y >= 0 && x < sizex && y < sizey;
}



bool jpspath::isjumppoint(int x, int y, int dirx, int diry) {
	return ispassible(x - dirx, y - diry, map) &&
		((ispassible(x + diry, y + dirx, map) &&
			iswall(x - dirx + diry, y - diry + dirx, map)) ||
			((ispassible(x - diry, y - dirx, map) &&
				iswall(x - dirx - diry, y - diry - dirx, map))));
}
void jpspath::precalc() {
	table = vector<vector<vector<int>>>(sizex, vector<vector<int>>(sizey, vector<int>(10, 0)));
	cout << " here";

	jps.clear();

	int num = 0;
	/*for (int i = 0; i < sizex; i++) {
		for (int j = 0; j < sizey; j++) {
			int x = i; //row
			int y = j; //col
			if ((*map)[x][y] == 0) { // road
				if (isjumppoint(x, y, 1, 0)) {
					table[x][y][8] |= dirdown;
					num++;
				}
				if (isjumppoint(x, y, -1, 0)) {
					table[x][y][8] |= dirup;
					num++;
				}
				if (isjumppoint(x, y, 0, 1)) {
					table[x][y][8] |= dirright;
					num++;
				}
				if (isjumppoint(x, y, 0, -1)) {
					table[x][y][8] |= dirleft;
					num++;
				}
			}
			else {

			}
		}
	}*
	/*
	for (int i = 0; i < sizex; i++) {
		for (int j = 0; j < sizey; j++) {
			if (ispassible(i, j, map)) {
				for (int d = 1; d < 8; d += 2) {
					int r = i + directions[d].x;
					int c = j + directions[d].y;

					if (0 > r || r >= sizex) continue;
					if (0 > c || c >= sizey) continue;

					if (iswall(r,c , map)) {
						if (arr[r][j] != 1 && arr[i][c] != 1) {
							table[i][j][8] = 1;
							convexhulls.push_back(vector<point>({point(i, j), point(i + 1, j), point(i + 1, j + 1), point(i, j + 1)}));
							num++;
						}
					}
				}
			}
		}
	}
	cout << "jp num : " << num << endl;

	bool flag;
	for (int i = 0; i < sizex; i++) {
		for (int j = 0; j < sizey; j++) {
			if (arr[i][j] != 1) {
				for (int d = 0; d < 8; d++) {
					int dist = 0;
					point dir = directions[d];
					point opdir1 = directions[(d + 3) % 8];
					point opdir2 = directions[(d + 5) % 8];

					for (int r = i + dir.x, c = j + dir.y; (0 <= r && r < sizex) && (0 <= c && c < sizey); r += dir.x, c += dir.y) {
						dist++;

						if (iswall(r, c, map)) {
							table[i][j][d] = (dist - 1) * -1;
							break;
						}
						if (d % 2 == 0) { // vertical, horizon
							if (table[r][c][8] != 0) {
								if (!isvalid(r + opdir1.x, c + opdir1.y, map)) {
									table[i][j][d] = dist;
									break;
								}
								if (!isvalid(r + opdir2.x, c + opdir2.y, map)) {
									table[i][j][d] = dist;
									break;
								}
								if (iswall(r + opdir1.x, c + opdir1.y, map) || iswall(r + opdir2.x, c + opdir2.y, map)) {
									table[i][j][d] = dist;
									break;
								}
							}

						}
						else { //diagonal
							flag = false;
							if (table[r][c][8] != 0) {
								table[i][j][d] = dist;
								break;
							}

							for (int rr = r + dir.x; 0 <= rr && rr < sizex; rr += dir.x) {
								if (arr[rr][c] == 1) {
									break;
								}
								else if (table[rr][c][8] != 0) {
									flag = true;
									break;
								}
							}

							for (int cc = c + dir.y; 0 <= cc && cc < sizey; cc += dir.y) {
								if (arr[r][cc] == 1) break;
								else if (table[r][cc][8] != 0) {
									flag = true;
									break;
								}
							}

							if (flag) {
								table[i][j][d] = dist;
								break;
							}

							if (0 > r + dir.x || r + dir.x >= sizex) {
								table[i][j][d] = (dist) * -1;
								break;
							}

							if (0 > c + dir.y || c + dir.y >= sizey) {
								table[i][j][d] = (dist) * -1;
								break;
							}

							if (arr[r + dir.x][c] == 1 || arr[r][c + dir.y] == 1) {
								table[i][j][d] = (dist) * -1;
								break;
							}
						}

						if ((r + dir.x == 0) || (r + dir.x == sizex) || (c + dir.y == 0) || (c + dir.y == sizey)) {
							table[i][j][d] = (dist) * -1;
						}
					}
				}
			}
		}
	}*/
	
	vector<point> directions = { {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, 1}, {-1, -1}, {1, -1} };

	for (int d = 0; d < 8; d++) {
		cout << "{" << d << endl;
		for (int i = 0; i < sizex; i++) {
			for (int j = 0; j < sizey; j++) {
				int x = i; //row
				int y = j; //col

				//cout << x << y << endl;
				if (iswall(x, y, map)) continue;
			
				int dirx = directions[d].x;
				int diry = directions[d].y;
				int dist = 0;
				int r = x;
				int c = y;
				while (true) {
					r += dirx, c += diry;
					dist++;
					
					if ((dirx != 0 && diry != 0)) { //not perfect but will not cause err
						if (iswall(r, c, map) || iswall(r - dirx, c, map) || iswall(r, c - diry, map)) {
							table[i][j][2 * (d - 4) + 1] = -(dist - 1);
							break;
						}
						if (dirx == 1 && diry == 1 && ispassible(r, c, map)) {//down right
							if ((table[r][c][0] > 0 || table[r][c][2] > 0) || table[r][c][8] != 0) {
								table[i][j][1] = dist;
								break;
							}
						}
						if (dirx == -1 && diry == 1 && ispassible(r, c, map)) {//up right
							if ((table[r][c][2] > 0 || table[r][c][4] > 0) || table[r][c][8] != 0) {
								table[i][j][3] = dist;
								break;
							}
						}
						if (dirx == -1 && diry == -1 && ispassible(r, c, map)) {//up left
							if ((table[r][c][4] > 0 || table[r][c][6] > 0) || table[r][c][8] != 0) {
								table[i][j][5] = dist;
								break;
							}
						}
						if (dirx == 1 && diry == -1 && ispassible(r, c, map)) {//downleft
							if ((table[r][c][0] > 0 || table[r][c][6] > 0) || table[r][c][8] != 0) {
								table[i][j][7] = dist;
								break;
							}
						}
						
					}
					if (d < 4) {
						if (iswall(r, c, map) || iswall(r - dirx, c, map) || iswall(r, c - diry, map)) {
							table[i][j][2 * d] = -(dist - 1);
							break;
						}

						if (dirx != 0 &&
							((ispassible(r, c + 1, map) && !ispassible(r - dirx, c + 1, map)) ||
								(ispassible(r, c - 1, map) && !ispassible(r - dirx, c - 1, map)))) {
							table[i][j][2 * d] = dist;
							if (table[r][c][8] == 0) {
								table[r][c][8] |= -1;
								num++;
								//convexhulls.push_back(vector<point>({ point(i, j), point(i + 1, j), point(i + 1, j + 1), point(i, j + 1) }));
								jps.push_back(point(r, c));
							}
							break;
						}
						if (diry != 0 &&
							((ispassible(r + 1, c, map) && !ispassible(r + 1, c - diry, map)) ||
								(ispassible(r - 1, c, map) && !ispassible(r - 1, c - diry, map)))) {
							table[i][j][2 * d] = dist;
							if (table[r][c][8] == 0) {
								table[r][c][8] |= -1;
								num++;
								//convexhulls.push_back(vector<point>({ point(i, j), point(i + 1, j), point(i + 1, j + 1), point(i, j + 1) }));
								jps.push_back(point(r, c));
							}
							break;
						}
					}
					
					
					/*cout << "awkward" << endl;
					cout << i << ":" << j << ":" << r << ":" << c << endl;*/
				}
			}
		}
	}

	cout << "jp num : " << num << endl;

	if (usingbounding) {
		boundingcalc();
	}
}

inline bool isbounding(point a) { // check if bounding to end
	
}
int ccw(point& a, point& b, point& c) {
	long long temp = 1ll * (b.x - a.x) * (c.y - a.y) - 1ll * (b.y - a.y) * (c.x - a.x);
	if (temp < 0) return -1;
	else if (temp == 0) return 0;
	else return 1;
}

//vector<int> clusters;
//vector<vector<point>> convexhulls; needed
void jpspath::boundingcalc() { 
	clusternumbers.clear();
	clusters.clear();
	convexhulls.clear();
	cboundtable.clear();
	
		


	//////////// useless stub
	/*vector<point> points;
	//dbscan
	for (int i = 0; i < sizex; i++) {
		for (int j = 0; j < sizey; j++) {
			if (arr[i][j] == 1) { // modied
				points.emplace_back(point(i, j));
			}
		}
	}

	double eps = 2; //need modify
	int minpts = 3;
	vector<int> clusternumbers = dbscan(points, eps, minpts);


	for (int i = 0; i < points.size(); i++) {
		if (clusternumbers[i] > 0) {
			clusters[clusternumbers[i]].emplace_back(points[i]);
		}
	}*/

	//////////////// bfs 

	queue<point> q;
	vector<vector<bool>> visited(sizex, vector<bool>(sizey, false));
	int clusternum = 0;

	for (int i = 0; i < sizex; i++) {
		for (int j = 0; j < sizey; j++) {
			if (iswall(i, j, map) && !visited[i][j]) {
				clusternum++;
				//unionfind[clusternum] = clusternum;

				clusters[clusternum].emplace_back(point(i, j));
				table[i][j][9] = clusternum;
				visited[i][j] = true;
				q.emplace(point(i, j));
				while (!q.empty()) {
					point& cur = q.front();
					q.pop();
					
					for (int d = 0; d < 8; d++) {
						int nx = cur.x + directions[d].x;
						int ny = cur.y + directions[d].y;
						if (!isvalid(nx, ny, map)) continue;
						if (ispassible(nx, ny, map) || visited[nx][ny]) continue;
						point np = point(nx, ny);
						visited[nx][ny] = true;
						clusters[clusternum].push_back(np);
						table[nx][ny][9] = clusternum;
						q.emplace(np);
					}
				}
			}
		}
	}

	/*for (auto& c : clusters) {
		convexhulls.push_back(c.second);
	}*/
	for (auto& c : clusters) {
		convexhulls.push_back(c.second);
		//auto box = getbbox(c.second, 5);
		/*for (int i = box[0].x; i <= box[2].x; i++) {
			for (int j = box[0].y; j <= box[2].y; j++) {
				if (table[i][j][9] != 0) {//9 is only for clusternum
					if (check(c.first, table[i][j][9])) continue;
					else union_pair(c.first, table[i][j][9]);
					table[i][j][9] = c.first;
				}
				
				table[i][j][9] = c.first;//?
			}

		}*/
		for (auto& p : c.second) {

			int boundingsize = 10;
			int minx = std::max(0, p.x - boundingsize);
			int miny = std::max(0, p.y - boundingsize);
			int maxx = std::min(sizex - 1, p.x + boundingsize);
			int maxy = std::min(sizey - 1, p.y + boundingsize);
			for (int i = minx; i <= maxx; i++) {
				for (int j = miny; j <= maxy; j++) {
					if (table[i][j][9] != 0) {//9 is only for clusternum
						//if (check(c.first, table[i][j][9])) continue;
						//else union_pair(c.first, table[i][j][9]);
						table[i][j][9] = c.first;
					}
				
					table[i][j][9] = c.first;//?
				}
			}
		}

		//convex hull
	}

	cout << "adjacent cluster : " << endl;
	for (int i = 1; i <= clusternum; i++) {
		//cout << unionfind[i] << " ";
	}
	cout << endl;

	cout << "number of cluster : " << clusters.size() << endl;
	//convex hull



	bool hist = usingbounding;
	usingbounding = false;
	int i = 0;
	int j = 0;
	int t = 0;

	unordered_map<int, vector<point>> jumpcluster;
	for (auto& p : jps) {
		//if getbbox() was 0 size
		/*for (auto& d : directions) {
			if (!isvalid(p.x + d.x, p.y + d.y, map)) continue;
			if (table[p.x + d.x][p.y + d.y][9] != 0) {
				int cnum = table[p.x + d.x][p.y + d.y][9];
				jumpcluster[cnum].emplace_back(p);
				table[p.x][p.y][9] = cnum;
			}
		}*/
		
		convexhulls.push_back(vector<point>({ p}));
		if (table[p.x][p.y][9] != 0) {
			int cnum = table[p.x][p.y][9];
			jumpcluster[cnum].emplace_back(p);
		}
	}

	for (auto& jc : jumpcluster) {
		for (auto& oc : jumpcluster) {
			if (jc.first >= oc.first) continue;
			vector<vec2> path;
			bool found = false;
			for (auto& p : jc.second) {
				if (found) break;
				for (auto& op : oc.second) {
					if (p == op) continue;
					//cout << "calcing: " << p.x << p.y << " ~ " << op.x << op.y << endl;

					this->setstart(vec2(p.x, p.y));
					this->setend(vec2(op.x, op.y));
					this->calculate();

					path = this->getResult();
					if (path.size() != 0) {

						found = true;
						break;
					}
				}
			}
			///calcs...
			if (!found) {

				//add that there is no path between them or not add?
				continue;
			}

			vector <point> chunk;
			chunk.insert(chunk.end(), jc.second.begin(), jc.second.end());
			chunk.insert(chunk.end(), oc.second.begin(), oc.second.end());
			for (auto i : path) {
				chunk.emplace_back(point(i.x, i.y));
			}

			auto bbox = getbbox(chunk, 1);
			cboundtable[jc.first * jumpcluster.size() + oc.first] = bbox/*bbox of 1 point of jc ~ 1point of oc path + jc + oc */;
			cboundtable[oc.first * jumpcluster.size() + jc.first] = bbox/*same as above*/;
			//if next point bounds in area bbox from     <-  cboundtable[table[c][c][9] * size + table[end][end][9]] then push or continue;

		}
	}

//	cout << "finished"


	usingbounding = hist;
	
}

void jpspath::expandCluster(vector<point>& points, point & p, int clusterid, double eps, int minpts, vector<int>& cluster) {
	vector<int> seeds;
	int i = 0;
	for (auto &o : points) {
		if (hypot(o.x - p.x, o.y - p.y) <= eps)
			seeds.emplace_back(i);
		i++;
	}

	if (seeds.size() < minpts) { //warning
		cluster[&p - &points[0]] = -1;
		return;
	}

	for (auto i : seeds) {
		cluster[i] = clusterid;
	}

	int idx = 0;
	while (idx < seeds.size()) {
		int curpoint = seeds[idx];
		vector<int> result;
		for (int i = 0; i < points.size(); i++) {
			if (hypot(points[curpoint].x - points[i].x, points[curpoint].y - points[i].y) <= eps) {
				result.push_back(i);
			}
		}

		if (result.size() >= minpts) {
			for (int i : result) {
				if (cluster[i] == -1 || cluster[i] == 0) {
					if (cluster[i] == -1)
						seeds.push_back(i);
					cluster[i] = clusterid;
				}
			}
		}
		idx++;
	}

	
	
}
vector<int> jpspath::dbscan(vector<point> &points, double eps, int minpts) {

	int clusterid = 1;
	vector<int> clusters(points.size(), 0);

	for (int i = 0; i < points.size(); i++) {
		if (clusters[i] == 0) {
			expandCluster(points, points[i], clusterid, eps, minpts, clusters);
			if (clusters[i] == clusterid) {
				clusterid++;
			}
		}
	}
	return clusters;
}

/// <summary>
/// monotone chain
/// </summary>
/// <param name="input"></param>
/// <returns></returns>
vector<point> jpspath::getconvex(vector<point>& input) {
	int n = input.size(), k = 0;
	if (n <= 3) return input;
	vector<point> hull(2 * n);

	sort(input.begin(), input.end(), [](const point& a, const point& b) {
		return a.x < b.x || (a.x == b.x && a.y < b.y);
		});

	for (int i = 0; i < n; ++i) {
		while (k >= 2 && (hull[k - 1].x - hull[k - 2].x) * (input[i].y - hull[k - 1].y) - (hull[k - 1].y - hull[k - 2].y) * (input[i].x - hull[k - 1].x) <= 0) k--;
		hull[k++] = input[i];
	}

	for (int i = n - 1, t = k + 1; i > 0; --i) {
		while (k >= t && (hull[k - 1].x - hull[k - 2].x) * (input[i - 1].y - hull[k - 1].y) - (hull[k - 1].y - hull[k - 2].y) * (input[i - 1].x - hull[k - 1].x) <= 0) k--;
		hull[k++] = input[i - 1];
	}

	hull.resize(k - 1);
	return hull;
}
vector<point> jpspath::getbbox(vector<point>& input, int size) {// simpler , get little bigger bbox of cluster
	
	int minx = 10000000, miny = 100000000;
	int maxx = -1, maxy = -1;
	for (auto& i : input) {
		if (minx > i.x) minx = i.x;
		if (miny > i.y) miny = i.y;
		if (maxx < i.x) maxx = i.x;
		if (maxy < i.y) maxy = i.y;
	}

	vector<point> res;
	res.push_back(point(std::max(minx - size, 0), std::max(miny - size, 0)));
	res.push_back(point(std::min(maxx + size, sizex - 1), std::max(miny - size, 0)));
	res.push_back(point(std::min(maxx + size, sizex - 1), std::min(maxy + size, sizey - 1)));
	res.push_back(point(std::max(minx - size, 0), std::min(maxy + size, sizey - 1)));
	/*res.push_back(point(minx, miny));
	res.push_back(point(maxx, miny));
	res.push_back(point(maxx, maxy));
	res.push_back(point(minx, maxy));*/
	return res;
}

bool jpspath::isinbbox(vector<point> box, point& pt) {
	return box[0].x <= pt.x && box[2].x >= pt.x && box[0].y <= pt.y && box[2].y >= pt.y;
}

vector<vector<vector<int>>> jpspath::gettable() {
	return table;
}

unordered_map<int, vector<point>> jpspath::getctable() {
	return cboundtable;
}
vector<point> jpspath::getjp() {
	return jps;
}


inline double jpspath::heuristic(point& a, point& b) {
	//return abs(a.x - b.x) + abs(a.y - b.y);
	return hypot(a.x - b.x, a.y - b.y);
	//return std::max(abs(a.x - b.x), abs(a.y - b.y)) + 1.41 * std::min(abs(a.x - b.x), abs(a.y - b.y)); //octile
}
//get direction to endpoint
inline point jpspath::jump(point cur, int idx) {
	point next = cur;
	point dir = directions[idx];
	int dist = table[next.x][next.y][idx]; // plus for next jp and neg for wall

	int absdist = dist > 0 ? dist : -dist;

	int diffrow = abs(cur.x - end.x);
	int diffcol = abs(cur.y - end.y);

	point toward_end_dir = point(0, 0);
	
	
	
	//double diff = hypot(diffrow, diffcol);
	if (idx % 2 == 1) {
		int smallerdiff = std::min(diffrow, diffcol);
		if (smallerdiff > absdist && dist < 0) return point{ -1, -1 };

		if (idx == 1) {
			if (cur.x < end.x && cur.y < end.y && smallerdiff <= absdist) {
				next.x += smallerdiff * dir.x;
				next.y += smallerdiff * dir.y;
				
				return next;
			}
			
		}
		if (idx == 3) {
			if (cur.x > end.x && cur.y < end.y && smallerdiff <= absdist) {
				next.x += smallerdiff * dir.x;
				next.y += smallerdiff * dir.y;
				
				return next;
			}
			
		}
		if (idx == 5) {
			if (cur.x > end.x && cur.y > end.y && smallerdiff <= absdist) {
				next.x += smallerdiff * dir.x;
				next.y += smallerdiff * dir.y;
				
				return next;
			}
	
		}
		if (idx == 7) {
			if (cur.x < end.x && cur.y > end.y && smallerdiff <= absdist) {
				next.x += smallerdiff * dir.x;
				next.y += smallerdiff * dir.y;
				
				return next;
			}
		}
	}
	else {
		if (idx == 0) {
			if (cur.y == end.y && cur.x < end.x && diffrow <= absdist) {
				next.x += diffrow * dir.x;
				
				return next;
			}
		}
		if (idx == 4) {
			if (cur.y == end.y && cur.x > end.x && diffrow <= absdist) {
				next.x += diffrow * dir.x;

				return next;
			}
		}
		if (idx == 2) {
			if (cur.x == end.x && cur.y < end.y && diffcol <= absdist) {
				next.y += diffcol * dir.y;
				
				return next;
			}
		}
		if (idx == 6) {
			if (cur.x == end.x && cur.y > end.y && diffcol <= absdist) {
				next.y += diffcol * dir.y;

				return next;
			}
		}
	}
	if (dist > 0) {
		next.x += dist * dir.x;
		next.y += dist * dir.y;
		
		return next;
		//return next;
	}
	return point{ -1, -1 };
	
	/*
	if (end.x != cur.x) { // need fix
		if (diffrow >= diffcol)
			toward_end_dir.x = end.x - cur.x > 0 ? 1 : -1;
	}
	if (end.y != cur.y) {
		if (diffrow <= diffcol)
			toward_end_dir.y = end.y - cur.y > 0 ? 1 : -1;
	}

	//double diff = hypot(cur.x - end.x, cur.y - end.y);
	//if (hypot(dir.x + toward_end_dir.x, dir.y + toward_end_dir.y) > 1) {
	
	
	//if (dir == toward_end_dir) {
	if (dir.x * toward_end_dir.x + dir.y * toward_end_dir.y >= 0) {
		int smallerdiff = std::max(diffrow, diffcol);
		
		if (smallerdiff <= absdist) {
			next.x += dir.x * smallerdiff;
			next.y += dir.y * smallerdiff;
			if (ispassible(next.x, next.y, map))
				return next;
		}
	}

	bool isproper = (dist > 0) ? true : false;

	//bool isproper =true;

	next.x += dir.x * absdist;
	next.y += dir.y * absdist;

	if (next == end) return end;

	isproper &= ispassible(next.x, next.y, map);

	return isproper ? next : point{ -1, -1 };*/
}

inline vector<point> jpspath::identify_nextpoints(Node* node) {
	//nexts.clear();
	int mindir = 0;
	int maxdir = 7;
	if (node->parent == nullptr) {
		mindir = 0;
		maxdir = 7;
	}
	else {
		point parent = node->parent->p;

		point searchdir = point(0, 0);
		if (parent.x < node->p.x) searchdir.x = 1;
		else if (parent.x == node->p.x) searchdir.x = 0;
		else searchdir.x = -1;

		if (parent.y < node->p.y) searchdir.y = 1;
		else if (parent.y == node->p.y) searchdir.y = 0;
		else searchdir.y = -1;

		int lastdir = 0;
		if (node->p.x == parent.x) {
			if (node->p.y > parent.y) {
				lastdir = 2;
			}
			else {
				lastdir = 6;
			}
		}
		else if (node->p.y == parent.y) {
			if (node->p.x > parent.x) {
				lastdir = 0;
			}
			else {
				lastdir = 4;
			}
		}
		else {
			if (node->p.y > parent.y) {
				if (node->p.x > parent.x) {
					lastdir = 1;
				}
				else {
					lastdir = 3;
				}
			} 
			else {
				if (node->p.x > parent.x) {
					lastdir = 7;
				}
				else {
					lastdir = 5;
				}
			}
		}

		point op1 = directions[(lastdir + 3) % 8];
		point op2 = directions[(lastdir + 5) % 8];
		mindir = (lastdir + 7);
		maxdir = (lastdir + 9);

		if (iswall(node->p.x + op1.x, node->p.y + op1.y, map) && iswall(node->p.x + op2.x, node->p.y + op2.y, map)) {
			mindir--;
			maxdir++;
		}
		else if (iswall(node->p.x + op1.x, node->p.y + op1.y, map)) {
			mindir++;
			maxdir++;
		}
		else if (iswall(node->p.x + op2.x, node->p.y + op2.y, map)) {
			mindir--;
			maxdir--;
		}

	}
	//cout << "dirs : "<<mindir << maxdir << endl;
	/*if (usingbounding)
		cout << "cur is : " << node->p.x << "," << node->p.y << endl;*/
	for (int i = mindir; i <= maxdir; i++) {
		int d = i % 8;
		//cout << d << "has jump dist" << table[node->p.x][node->p.y][d] << endl;
		point jumppoint = jump(node->p, d);
		//cout << jumppoint.x << " " << jumppoint.y << endl;
		
		if (usingbounding) { //if next is jp, if next bounds in     cur : end path cluster bbox then return or continue.
			if (jumppoint.x == -1) continue;
			//cout << "promising" << jumppoint.x << jumppoint.y << endl;
			//if ()

			//if next point bounds in area bbox from     <-  cboundtable[table[c][c][9] * size + table[end][end][9]] then push or continue;
			//fi (isinbbox())
			int var1 = table[node->p.x][node->p.y][9];
			int var2 = table[end.x][end.y][9];
			int isjp = table[node->p.x][node->p.y][8];
			//cout << "cur values : " << var1 <<" : " << var2 << " : " << isjp << endl;
			if (var1 == var2 || var1 == 0 || var2 == 0 || isjp == 0) {
				//cout << "selected" << jumppoint.x << ":" << jumppoint.y << endl;
				nexts.emplace_back(jumppoint);
				continue;
			}

			int hash = var1 * clusters.size() + var2; //modify 68
			if (!cboundtable.count(hash)) continue;
			vector<point> &box = cboundtable[hash];
			if (!isinbbox(box, end)) continue;

			/*cout << "hash was" << hash / clusters.size() << ": " << hash % clusters.size() << endl;
			cout << jumppoint.x << " : " << jumppoint.y << endl;*/
			nexts.emplace_back(jumppoint);
		}
		if (!usingbounding) {
			if (jumppoint.x != -1) {
				//cout << mindir << ":" << maxdir << endl;
				//cout << jumppoint.x << " " << jumppoint.y << endl;
				nexts.emplace_back(jumppoint);
			}
		}

	}
	//cout << endl;
	return nexts;
}

/*void jpspath::calculate() { //slow, perfect
	//memset(openlist, 0, sizeof(openlist));
	//pq = priority_queue <Node, vector<Node>, less<Node>>(less<Node>(), move(pqreserve));
	closelist.clear();

	priority_queue < Node*, vector<Node*>, function<bool(Node*, Node*)>> pq([](Node* a, Node* b) ->bool {return *a > *b;});
	//cout << "end is in" << table[end.x][end.y][9] << endl;
	Node& nodestart = openlist[start.x][start.y];

	nodestart.p = start;
	nodestart.g = 0;
	nodestart.h = heuristic(start, end);
	nodestart.status = 1;
	nodestart.parent = nullptr;

	pq.emplace(&nodestart);

	found = false;
	Node* last = nullptr;
	while (!pq.empty()) {
		closelist.emplace_back(pq.top());
		Node* cur = pq.top();
		pq.pop();
		if (cur->status == 2) continue;
		cur->status = 2;

		if (cur->p == end) {
			last = cur;
			found = true;
			break;
		}

		identify_nextpoints(cur);

		while (!nexts.empty()) {
			point& next = *(nexts.end() - 1);
			nexts.pop_back();
			Node& nextnode = openlist[next.x][next.y];
			if (nextnode.status == 2) continue;

			double newg = cur->g + heuristic(cur->p, next);

			
			if (nextnode.status == 0) {
				nextnode.p = next;
				nextnode.g = newg;
				nextnode.h = heuristic(next, end);
				nextnode.parent = cur;
				nextnode.status = 1;
				pq.emplace(&nextnode);
			}
			else if (newg < nextnode.g) {
				nextnode.p = next;
				nextnode.g = newg;
				nextnode.h = heuristic(next, end);
				nextnode.parent = cur;
				nextnode.status = 1;
				//closelist.emplace_back(&openlist[next.x][next.y]);
				pq.emplace(&nextnode);
			}
		}
	}
	if (found) {
		vector<vec2> path;
		Node* c, *p;
		int cidx = closelist.size() - 1;
		c = closelist.back();
		p = closelist.back()->parent;
		path.emplace_back(vec2(c->p.x, c->p.y));
		while (cidx >= 0) {
			c = &openlist[closelist[cidx]->p.x][closelist[cidx]->p.y];
			c->g = 0;
			c->h = 0;
			c->status = 0;
			if (c == p) {
				path.emplace_back(vec2(c->p.x, c->p.y));
				p = c->parent;
			}
			//c->parent = nullptr;
			cidx--;
		}
		path.emplace_back(vec2(c->p.x, c->p.y));
		
		for (Node* node = last; node; node = node->parent)
			path.emplace_back(vec2(node->p.x, node->p.y
			
		reverse(path.begin(), path.end());
		results = path;
		//openlist = vector<vector<Node>>(sizex, vector<Node>(sizey, Node()));
	}
	else {
		openlist = vector<vector<Node>>(sizex, vector<Node>(sizey, Node()));
	}
}*/
/// <summary>
/// O cast : 20ms for non exist path 4ms for exist slowest for 20 entities    fastest : 0.5ms <- 20entities
/// </summary>
void jpspath::calculate() {
	nodes.clear();
	nexts.clear();
	
	auto hashfunc = [](point& p) { /// node ´Ã¾î³ª¸é ÀÌ°Å ¹Ù²ã¾ßµÊ -> Áß¿ä
		return p.x * 10000 + p.y;
	};
	auto getNode = [&](point& p) -> Node& { return nodes[hashfunc(p)]; };

	priority_queue < Node, vector<Node>, greater<Node>> pq(greater<Node>(), move(pqreserve)); //openlist
	/*function<bool(Node*, Node*)> f = [](Node* a, Node* b) -> bool {
		return *a > *b;
	};
	priority_queue < Node*, vector<Node*>, function<bool(Node*, Node*)>> pq(f, move(pqreserve)); *///pqreserve must be vector<Node*>
	//pq = priority_queue < Node, vector<Node>, greater<Node>>();
	//nodes.reserve(1000000);
	
	nodes[hashfunc(start)] = Node(start, 0, 0, heuristic(start, end), nullptr);
	
	pq.push(nodes[hashfunc(start)]);
	found = false;

	while (!pq.empty()) {
		//Node cur = pq.top();
		point p = pq.top().p;
		Node cur = getNode(p);
		//
		pq.pop();
		if (cur.status == 1) continue;
		cur.status = 1;
		//cout << cur.p.x << " " << cur.p.y << endl;

		if (cur.p == end) {
			vector<vec2> path;
			for (Node* node = &cur, *prev = &cur; node; node = node->parent, prev = node) {
				path.emplace_back(vec2(node->p.x, node->p.y));
				//delete prev;
			}

			reverse(path.begin(), path.end());
			results = path;
			found = true;
			return;
		}
		
		identify_nextpoints(&cur);
		
		while (!nexts.empty()) {

			point& next = *(nexts.end() - 1);
			nexts.pop_back();

			double newg = cur.g + heuristic(cur.p, next);
			int hash = hashfunc(next);

			bool exists = nodes.count(hash);
			if (exists && nodes[hash].status == 1) continue;

			if (!exists || newg < nodes[hash].g) { //open and newcost is smaller than old
				Node node = Node(next, 0, newg, heuristic(next, end), &getNode(cur.p));
				nodes[hash] = node;
				pq.emplace(node);
			}

		}
		/*for (auto& next : identify_nextpoints(&cur)) {
			double newg = cur.g + heuristic(cur.p, next);
			int hash = hashfunc(next);
			if (!nodes.count(hash) || newg < getNode(next).g) {
				Node node = Node(next, newg, heuristic(next, end), &getNode(cur.p));
				nodes[hash] = node;
				pq.push(node);
			}
		}*/
		
		
	}
}

/// <summary>
/// pointer version of calc. same speed as above, works well
/// turning on heap profiling will make this thing dramatically slow : 350ms
/// </summary>
/// 
/*void jpspath::calculate() {
	nodes.clear();
	nexts.clear();

	/// <summary>
	/// closelist ?
	/// </summary>
	/// 
	//vector<Node> closelist = {};

	auto hashfunc = [](point& p) { /// node ´Ã¾î³ª¸é ÀÌ°Å ¹Ù²ã¾ßµÊ -> Áß¿ä
		return p.x * 10000 + p.y;
	};
	auto getNode = [&](point& p) -> Node* { return nodes[hashfunc(p)]; };

	//priority_queue < Node, vector<Node>, less<Node>> pq(less<Node>(), move(pqreserve)); //openlist
	function<bool(Node*, Node*)> f = [](Node* a, Node* b) -> bool {
		return *a > *b;
	};
	priority_queue < Node*, vector<Node*>, function<bool(Node*, Node*)>> pq(f, move(pqreserve)); //pqreserve must be vector<Node*>
	//pq = priority_queue < Node, vector<Node>, greater<Node>>();
	//nodes.reserve(1000000);

	nodes[hashfunc(start)] = new Node(start, 0, 0, heuristic(start, end), nullptr);

	pq.push(nodes[hashfunc(start)]);
	found = false;

	while (!pq.empty()) {
		//Node cur = pq.top();
		Node* cur = pq.top();
		//
		pq.pop();
		if (cur->status == 1) continue;
		cur->status = 1;
		//cout << cur.p.x << " " << cur.p.y << endl;

		if (cur->p == end) {
			vector<vec2> path;
			for (Node* node = cur, *prev = cur; node; node = node->parent, prev = node) {
				path.emplace_back(vec2(node->p.x, node->p.y));
				delete prev;
			}

			reverse(path.begin(), path.end());
			results = path;
			found = true;
			return;
		}

		identify_nextpoints(cur);

		while (!nexts.empty()) {

			point& next = *(nexts.end() - 1);
			nexts.pop_back();

			double newg = cur->g + heuristic(cur->p, next);
			int hash = hashfunc(next);

			bool exists = nodes.count(hash);
			if (exists && nodes[hash]->status == 1) continue;

			if (!exists || newg < nodes[hash]->g) { //open and newcost is smaller than old
				Node* node = new Node(next, 0, newg, heuristic(next, end), getNode(cur->p));
				nodes[hash] = node;
				pq.emplace(node);
			}
			/*else {
				Node* node = nodes[hash];
				if (newg < node->g) {
					node->g = newg;
					node->p = next; //important to node making pathfind
					node->h = heuristic(next, end);
					node->parent = getNode(cur->p);
					node->status = 0;
					pq.emplace(node);
				}
			}
		}
		/*for (auto& next : identify_nextpoints(&cur)) {
			double newg = cur.g + heuristic(cur.p, next);
			int hash = hashfunc(next);
			if (!nodes.count(hash) || newg < getNode(next).g) {
				Node node = Node(next, newg, heuristic(next, end), &getNode(cur.p));
				nodes[hash] = node;
				pq.push(node);
			}
		}

	}
}*/



//this thing works properly, but slow : 20entity 60ms
/*void jpspath::calculate() {
	nodes.clear();
	closelist.clear();
	auto hashfunc = [](point& p) {
		return p.x * 1024 + p.y;
	};

	//openlist = vector<vector<Node>>(sizex, vector<Node>(sizey, Node()));
	//auto getNode = [&](point& p) -> Node& { return *nodes[hashfunc(p)]; };

	priority_queue < Node, vector<Node>, less<Node>> pq(less<Node>(), move(pqreserve)); //openlist
	//pq = priority_queue < Node, vector<Node>, greater<Node>>();
	//nodes.reserve(1000000);

	//nodes[hashfunc(start)] = Node(start, 0, heuristic(start, end));
	Node& startnode = openlist[start.x][start.y];
	startnode.p = start;
	startnode.g = 0;
	startnode.h = heuristic(start, end);
	startnode.status = 1;
	startnode.parent = nullptr;
	nodes[hashfunc(start)] = &startnode;
	
	
	pq.emplace(startnode);
	found = false;


	while (!pq.empty() && pq.size() < 100000) {
		Node* cur = &openlist[pq.top().p.x][pq.top().p.y];
		pq.pop();

		if (cur->status == 2) continue;
		cur->status = 2;
		//cout << cur.p.x << " " << cur.p.y << endl;

		if (cur->p == end) {
			vector<vec2> path;
			for (Node* node = cur; node; node = node->parent)
				path.emplace_back(vec2(node->p.x, node->p.y));
			
			for (auto c : nodes) {
				Node& node = openlist[c.second->p.x][c.second->p.y];
				node.g = 0;
				node.h = 0;
				node.parent = nullptr;
				node.status = 0;
			}
			reverse(path.begin(), path.end());
			openlist = vector<vector<Node>>(sizex, vector<Node>(sizey, Node()));
			results = path;
			found = true;
			return;
		}
		
		identify_nextpoints(cur);
		
		while (!nexts.empty()) {

			point next = *(nexts.end() - 1);
			nexts.pop_back();

			double newg = cur->g + heuristic(cur->p, next);
			
			int hash = hashfunc(next);

			if (openlist[next.x][next.y].status == 2) continue;
			
			if (openlist[next.x][next.y].status == 0) { //open and newcost is smaller than old
				Node& node = openlist[next.x][next.y];
				node.p = next;
				node.status = 1;
				node.g = newg;
				node.h = heuristic(next, end);
				node.parent = &openlist[cur->p.x][cur->p.y];
					//Node(next, 0, newg, heuristic(next, end), &nodes[hashfunc(cur.p)]);
				nodes[hash] = &node;
				pq.emplace(node);
				//closelist.emplace_back(&node);
			}
			else {
				Node& node = openlist[next.x][next.y];
				if (newg < node.g) {
					node.p = next;
					node.status = 1;
					node.g = newg;
					node.h = heuristic(next, end);
					node.parent = &openlist[cur->p.x][cur->p.y];
					nodes[hash] = &node;
					pq.emplace(node);
					//closelist.emplace_back(&node);
				}
			}

			
		}
		/*for (auto& next : identify_nextpoints(&cur)) {
			double newg = cur.g + heuristic(cur.p, next);
			int hash = hashfunc(next);
			if (!nodes.count(hash) || newg < getNode(next).g) {
				Node node = Node(next, newg, heuristic(next, end), &getNode(cur.p));
				nodes[hash] = node;
				pq.push(node);
			}
		}
	}
	openlist = vector<vector<Node>>(sizex, vector<Node>(sizey, Node()));
}*/

bool jpspath::lineofsight(vec2 posa, vec2 posb) { //true º¸ÀÓ false º®ÀÌ¶û Ãæµ¹
	int x0 = (int)posa.x, y0 = (int)posa.y, x1 = (int)posb.x, y1 = (int)posb.y;
	int dx = abs(x1 - x0), dy = abs(y1 - y0);
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;
	int err = dx - dy;
	int e2;
	int x = x0, y = y0;
	while (true) {
		if (!ispassible(x, y, map)) return false;
		if (x == x1 && y == y1) break;
		e2 = err << 1;
		if (e2 > -dy) { err -= dy; x += sx; }
		if (e2 < dx) { err += dx; y += sy; }
	}
	return true;
}
void jpspath::postsmooth() {
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