#pragma once
#include "pathfinders.h"
//struct JumpPoint {
//	int dist;
//	bool hasjumppointondir;
//};
//vector<point> directions = { {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1} };
enum diridx : int {
	dirdown = 1, dirright = 2, dirleft = 4, dirup = 8
};

class jpspath : public pathfinders
{
public:
	jpspath();
	~jpspath();
	void calculate();
	void loadTable();
	void saveTable();
	void precalc();
	void boundingcalc();
	void changemod(bool usingbounding);
	vector<vector<point>> &getclusters() {
		return convexhulls;
	}
	void postsmooth();
	vector<vector<vector<int>>> gettable();
	//void bakemap();
	bool usingbounding;

	vector<point> getbbox(vector<point>& inputs, int size);
	bool isinbbox(vector<point> box, point& pt);

	unordered_map<int, vector<point>> getctable();
	vector<point> getjp();
	
private:
	inline point jump(point cur, int idx);
	inline bool iswall(int x, int y, const vector<vector<int>>* map);
	inline bool ispassible(int x, int y, const vector<vector<int>>*map);
	inline bool isvalid(int x, int y, const vector<vector<int>>*map);
	inline double heuristic(point &a, point &b);
	inline vector<point> identify_nextpoints(Node * node);
	inline bool isinbox(point target);

	bool lineofsight(vec2 posa, vec2 posb);
	bool isjumppoint(int x, int y, int dirx, int diry);

	vector<int> dbscan(vector<point> &points, double eps, int minpts);
	vector<point> getconvex(vector<point>& inputs);
	
	void expandCluster(vector<point>& points, point& p, int clusterid, double eps, int minpts, vector<int>& cluster);

	/*int find(int clusternum) {
		if (unionfind[clusternum] == clusternum) return clusternum;
		else {
			return unionfind[clusternum] = find(unionfind[clusternum]);
		}
	}
	void union_pair(int a, int b) {
		a = find(a);
		b = find(b);
		unionfind[b] = a;
	}
	int check(int a, int b) {
		return find(a) == find(b) ? 1 : 0;
	}*/

	vector<vector<int>>* map;
	vector<vector<vector<int>>> table;
	vector<vector<Node>> openlist;
	//Node openlist[sizex][sizey];
	

	/// <summary>
	/// jpsvars
	/// </summary>
	vector<Node> pqreserve;
	unordered_map<int, Node> nodes; //closedlist
	///unordered_map<int, Node*> nodes;
	//priority_queue<Node, vector<Node>, less<Node>> pq;
	vector<point> nexts;
	vector<Node*> closelist;

	unordered_map<int, vector<point>> cboundtable;//using now


	/// <summary>
	/// dbscan vars
	/// dbscan seems useless
	/// </summary>
	vector<int> clusternumbers; //deprecated?


	vector<vector<point>> convexhulls;
	unordered_map<int, vector<point>> clusters;
	

	vector<point> jps;//for show on screen
};

