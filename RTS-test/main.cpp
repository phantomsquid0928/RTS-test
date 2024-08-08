#include "headers.hpp"
#include "mouse.h"
#include "shader/shader.h"
#include "block.h"
#include "entity.h"
#include "pathfinders.h"
#include "astarpath.h"
#include "path.h"
#include "wall.h"
#include "jpspath.h"
#include "bbox.h"
#include "text.h"
#include "flowfield.h"

int loadMap();
int saveMap();



void keyhandler(GLFWwindow* window, int key, int code, int action, int mode);
void mousehandler(GLFWwindow* window, double xpos, double ypos);
void clickhandler(GLFWwindow* window, int button, int action, int mods);


void tick(int * count);
void makeBlocks();
void makeEntities();


string readFile(const char *filepath);
GLuint createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);

namespace input {
	
	int mousex;
	int mousey;
	int clickposx;
	int clickposy;
	bool togglebbox;
	double timeold;
	double timenow;
	double recenttime;
	status keys_status[keys_len];
	status mouse_status[mouse_len];
	bool mode_status[mode_len];
}
namespace mapinfo {
	vector<vector<int>> arr(sizex / cellsize, vector<int> (sizey / cellsize, 0));
	//int arr[sizey / cellsize][sizex / cellsize] = { 0 };
	//vector<array<int, 3>> blocklist;
	vector<array<float, 3>> entitylist;
	vector<array<float, 3>> destlist;
	vector<point> walllist;

	vector<vector<point>> hulls;
	vector<vector<point>> boxes;
	bitset<100> selected; //TODO: change this after
	
	int radius = 10;


}

pathfinders *pf;
astarpath *a;
jpspath* j;
flowfield* f;
path* p;


entity *entities;
wall* walls;
bbox* bboxes;
text* textmanager;


//vector<entity> entitylist;

using namespace input;
using namespace mapinfo;

int main() {
	glewExperimental = GL_TRUE;

	if (!glfwInit()) {
		fprintf(stderr, "failed to init GLFW");
		return -1;
	}

	//glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	

	GLFWwindow* window;

	window = glfwCreateWindow(fixedy, fixedx, "____", nullptr, nullptr); // / 4 on sizes when using 4096

	glfwMakeContextCurrent(window);

	//glEnable(GL_CULL_FACE); <- ??? this will erase all, entities,text,  walls but not bboxes and mouse. 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   //these two makes text appear well fk

	//set pf method as astar
	pf = new pathfinders();
	f = new flowfield();
	j = new jpspath();
	a = new astarpath();
	pf->setfunc(a);
	//pf->setfunc(j);

	//callbacks
	glfwSetKeyCallback(window, keyhandler);
	glfwSetMouseButtonCallback(window, clickhandler);
	glfwSetCursorPosCallback(window, mousehandler);

	

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);

	glViewport(0, 0, bufferWidth, bufferHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, bufferWidth, 0.0, bufferHeight, -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	glewInit();


	mouse m = mouse();
	m.render();

	//sizex = 9;
	//sizey = 5;
	//
	/*cout << endl;
	arr = {{0, 0, 1, 0, 0, 0, 1, 0, 0},
			{0, 0, 0, 0, 0, 0, 1, 0, 0 },
			{0, 1, 1, 0, 0, 0, 1, 1, 0 },
			{0, 0, 1, 0, 0, 0, 0, 0, 0 },
			{0, 0, 1, 0, 0, 0, 1, 0, 0 }, };

	
	/*pf->setfunc(f);
	pf->setend(vec2(3, 3));
	pf->calculate();

	vector<vector<flownode>> resfield = f->getfield();
	for (auto &i : resfield) {
		for (auto& j : i) {
			if (j.bestcost == INT_MAX) cout << "-" << " ";
			else cout << j.bestcost << " ";
		}
		cout << endl;
	}
	j->precalc(); //jps need precalc.
	j->saveTable();
	j->loadTable();
	vector<vector<vector<int>>> t = j->gettable();
	vector<vector<int>> jpm(3 * sizex, vector<int>(3 * sizey, 0));

	for (int x = 0; x < sizex; x++) {
		for (int y = 0; y < sizey; y++) {
			jpm[0 + x * 3][0 + y * 3] = t[x][y][5];
			jpm[0 + x * 3][1 + y * 3] = t[x][y][4];
			jpm[0 + x * 3][2 + y * 3] = t[x][y][3];
			jpm[1 + x * 3][0 + y * 3] = t[x][y][6];
			jpm[1 + x * 3][1 + y * 3] = 0;//t[x][y][8];
			jpm[1 + x * 3][2 + y * 3] = t[x][y][2];
			jpm[2 + x * 3][0 + y * 3] = t[x][y][7];
			jpm[2 + x * 3][1 + y * 3] = t[x][y][0];
			jpm[2 + x * 3][2 + y * 3] = t[x][y][1];
		}
	}

	int a = 0;
	for (auto &i : jpm) {
		int k = 0;
		for (auto &j : i) {
			if (k % 3 == 0) printf("|");

			printf("%3d", j);
			k++;
		}
		a++;
		if (a % 3 == 0) {
			cout << endl;
			for (int b = 0; b < 5 * sizey; b++)
				printf("ㅡ");
		}
		cout << endl;
	}*/




	//sizex = 1024;
	//sizey = 1024;
	loadMap();

	double usedtime = 0;
	filesystem::path filePath("jpbakedmap.dat");
	if (filesystem::exists(filePath)) {
		cout << "existing, loading precalculated table. if u changed mapsize, then discard that file";
		j->loadTable();
	}
	else {
		cout << "no precalced file, calculating...";
		chrono::system_clock::time_point start;
		chrono::system_clock::time_point end;
		start = chrono::system_clock::now();
		j->precalc(); //jps need precalc.

		end = chrono::system_clock::now();
		typedef chrono::duration<double, std::milli> millisec;
		
		usedtime = chrono::duration_cast<millisec> (end - start).count();
		cout << "precalc time : " << usedtime << " in ms" << endl;
	}
	//j->boundingcalc();
	auto res = j->getctable();
	auto jps = j->getjp();
	auto jpsorigin = j->getclusters(); //debug

	int num = 0;

	for (auto& i : res) {
		vector<point> box = j->getbbox(i.second, 1);

		cout << "[";
		for (auto& a : box) {
			cout << "(" << a.x << ", " << a.y << "), ";
		}
		boxes.push_back(move(box));
		cout << "]" << endl;
		num++;
	}

	/*for (auto& p : jps) {
		boxes.push_back(vector<point>({ p, point(p.x + 1, p.y), point(p.x + 1, p.y + 1), point(p.x, p.y + 1) }));
	}*/
	for (auto& box : jpsorigin) {
		boxes.push_back(move(box));
	}
	
	cout << "area num : " << num << endl;

	bboxes = new bbox(&boxes);
	//loadEntities();
	bboxes->create();

	//makeBlocks();
	makeEntities();
	selected.reset();

	

	walls = new wall();
	walls->create();

	entities = new entity(entitylist, f);
	entities->create();

	mode_status[rvo] = true;
	int e = 36;
	for (int i = 0; i < e; i++) {
		int x = entitylist[i][0];
		int y = entitylist[i][1];

		int destx = 250 + 150 * cos(radians((float)10 * i + 180));
		int desty = 250 + 150 * sin(radians((float)10 * i + 180));

		vector<vec2>* path = new vector<vec2>();
		path->push_back(vec2(x, y));
		path->push_back(vec2(destx, desty));

		entities->setpath(i, path);
	}
	p = new path();
	p->create();

	textmanager = new text("shader/arial.ttf");

	
	textmanager->addText(0, "astar     spent: NONE", vec4(0, 1, 1, 1), vec2(800 * zoomx, 950 * zoomy), 0.3f * zoomx);
	textmanager->addText(1, "jps+      spent: NONE", vec4(0, 1, 1, 1), vec2(800 * zoomx, 975 * zoomy), 0.3f * zoomx);
	textmanager->addText(2, "jps+ goal spent: NONE", vec4(0, 1, 1, 1), vec2(800 * zoomx, 1000 * zoomy), 0.3f * zoomx);
	textmanager->addText(3, "flowfield spent: NONE", vec4(0, 1, 1, 1), vec2(800 * zoomx, 925 * zoomy), 0.3f * zoomx);
	textmanager->addText(4, "current mode: jpspath", vec4(0, 1, 1, 1), vec2(10 * zoomx, 1000 * zoomy), 0.3f * zoomx);
	textmanager->addText(5, "precalc time was : " + to_string(usedtime), vec4(0, 1, 1, 1), vec2(10 * zoomx, 975 * zoomy), 0.3f * zoomx);
	/*auto chars = textmanager->getLoadedChars();
	for (auto &c : chars) {
		cout << (char)c.first << c.second.TextureID << endl;
	}*/

	recenttime = 0;

	//
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	timeold = glfwGetTime();
	int count = 0;
	do {
		
		glClear(GL_COLOR_BUFFER_BIT);
		//cout << (double)count / (timenow - timeold) << endl; //fps
		
		tick(&count);
		entities->render();
		
		//textmanager.renderOne("hello", 100.0f, 100.0f, 1.0f, vec3(0, 1.f, 1.f));
		//entities->
		int i = 0;


		walls->render();
		m.update();
		textmanager->render();

		if (togglebbox)
			bboxes->render();
		if ((keys_status[LALT] == PRESS || keys_status[LCTRL] == PRESS) && (mouse_status[leftmouse] == PRESS || mouse_status[leftmouse] == DRAGGING))
			walls->update();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		count++;
	} while (glfwGetKey(window, GLFW_KEY_DELETE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	saveMap();
	j->saveTable();
	glfwDestroyWindow(window);
	glfwTerminate();
}

void tick(int * count) { //will update entities location by calling entities.update();
	timenow = glfwGetTime();
	
	double delta = timenow - timeold;
	recenttime += timenow - timeold;
	timeold = timenow;
	int i = 0;
	bool changed = false;

	entities->update(delta);
	for (auto path : entities->getentitypaths()) { // not good
		if (path == nullptr) {
			i++;
			continue;
		}
		p->setpath(*path);

		changed = true;
		p->render();
		i++;
		
	}
	if (changed) recenttime = 0;
}

//load, save


int loadMap() {
	ifstream ifs;
	ifs.open(mapfile);

	if (!ifs.is_open()) {
		return -1;
	}

	string input = "";
	int idy = 0;

	while (!ifs.eof()) {
		getline(ifs, input);
		int idx = 0;
		for (auto i : input) {
			if (isdigit(i)) {
  				arr[idy][idx] = i - '0'; //wall = 1 now
				if (arr[idy][idx] == 1) {
					for (int k = std::max(0, idy - radius); k < std::min(sizex, idy + radius); k++) {
						for (int t = std::max(0, idx - radius); t < std::min(sizey, idx + radius); t++) {
							if (arr[k][t] != 1) arr[k][t] = 2;
						}
					}
				}
			}
			else {
				switch (i) {
				case 'w':
					arr[idy][idx] = -1;
					break;
				default:
					arr[idy][idx] = 0;
					break;
				}
			}
			idx++;
		}
		idy++;
	}
	
	ifs.close();
	return 0;
}
int saveMap() { 
	ofstream ofs;
	ofs.open(mapfile, ios::out | ios::trunc);

	for (int i = 0; i < sizex / cellsize; i++) {
		for (int j = 0; j < sizey / cellsize; j++) {
			if (arr[i][j] < 0) { // wall
				ofs << "1";
			}
			else {
				if (arr[i][j] == 2) {
					ofs << 0;
				}
				else 
					ofs << arr[i][j];
			}
		}
		ofs << endl;
	}
	ofs.close();
	return 0;
}

//////handlers... (callback)

void keyhandler(GLFWwindow* window, int key, int code, int action, int mode) {
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		cout << "s pressed" << endl;
		togglebbox ^= true;
		keys_status[S] = PRESS;
	}
	if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		cout << "s released" << endl;
		keys_status[S] = RELEASE;
	}
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
		cout << "lctrl pressed, erasing wall mode on" << endl;
		
		keys_status[LCTRL] = PRESS;
	}
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) { //need precalc on changed map
		cout << "lctrl released" << endl;
		keys_status[LCTRL] = RELEASE;
		
		if (j) {
			j->precalc();

			auto res = j->getctable();
			auto jps = j->getjp();

			int num = 0;
			boxes.clear();

			for (auto& i : res) {
				vector<point> box = j->getbbox(i.second, 1);

				cout << "[";
				for (auto& a : box) {
					cout << "(" << a.x << ", " << a.y << "), ";
				}
				boxes.push_back(move(box));
				cout << "]" << endl;
				num++;
			}

			for (auto& p : jps) {
				boxes.push_back(vector<point>({ p, point(p.x + 1, p.y), point(p.x + 1, p.y + 1), point(p.x, p.y + 1) }));
			}

			bboxes->update();
		}
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		cout << "ESC pressed, clear bits" << endl;
		keys_status[ESC] = PRESS;
		arr = vector<vector<int>> (sizex / cellsize, vector<int>(sizey / cellsize, 0));
		selected.reset();
		walls->update();
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		cout << "ESC released" << endl;
		keys_status[ESC] = RELEASE;
	}
	if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
		cout << "ALT pressed, adding wall mode on" << endl;
		keys_status[LALT] = PRESS;

	}
	if (key == GLFW_KEY_LEFT_ALT && action == GLFW_RELEASE) { //need precalc on changed map
		cout << "ALT released" << endl;
	
		keys_status[LALT] = RELEASE;
		if (j) {
			j->precalc();

			auto res = j->getctable();
			auto jps = j->getjp();

			int num = 0;
			boxes.clear();

			for (auto& i : res) {
				vector<point> box = j->getbbox(i.second, 1);

				cout << "[";
				for (auto& a : box) {
					cout << "(" << a.x << ", " << a.y << "), ";
				}
				boxes.push_back(move(box));
				cout << "]" << endl;
				num++;
			}

			for (auto& p : jps) {
				boxes.push_back(vector<point>({ p, point(p.x + 1, p.y), point(p.x + 1, p.y + 1), point(p.x, p.y + 1) }));
			}

			bboxes->update();
		}
	}


	//mode handling
	if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
		cout << "f released, flow field on" << endl;
		//eys_status[LALT] = PRESS;
		mode_status[flowmode] ^= true;
		textmanager->addText(4, (mode_status[flowmode] == true) ? "current mode : flowfield" : "current mode : jpspath", vec4(0, 1, 1, 1), vec2(10, 1000), 0.3f);
		
		//mode_status[pathmode] ^= true;
	}
	if (key == GLFW_KEY_R && action == GLFW_RELEASE) {
		cout << (mode_status[rvo] == false ? "RVO on" : "RVO off") << endl;
		//keys_status[ESC] = RELEASE;
		mode_status[rvo] ^= true;
	}
	
}
void mousehandler(GLFWwindow* window, double xpos, double ypos) { //움직일때만 콜백이구만
	mousex = zoomx * ypos;
	mousey = zoomy * xpos; /// 4 * xpos , 4 * ypos when using shrinkage
	//cout << xpos << ypos << endl;
	//TODO: mouse escape? <
	if (mouse_status[leftmouse] == PRESS || mouse_status[leftmouse] == DRAGGING) { //drag // works!
		//cout << "is dragging, " << mousex << " , " << mousey << " ~ " << clickposx << ", " << clickposy << endl;
		mouse_status[leftmouse] = DRAGGING;
		
		//render rectangle, write this on other func area
	}
	else {
		mouse_status[leftmouse] = NON;
	}
}
void clickhandler(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
		clickposx = mousex;
		clickposy = mousey;
		cout << "lt click x : " << mousex << "y : " << mousey << endl;
		mouse_status[leftmouse] = PRESS;
	}
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
		cout << "lt rel   x : " << mousex << "y : " << mousey << endl;
		mouse_status[leftmouse] = RELEASE;
	}
	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) { //will make direction to... : by thread? pthread
		clickposx = mousex;
		clickposy = mousey; //depr
		cout << "rt click x : " << mousex << "y : " << mousey << endl;
		mouse_status[rightmouse] = PRESS;

		double astartotal = 0;
		double jpstotal = 0;
		double jpsgoal = 0;
		double flowspent = 0;
		pf->setfunc(f);
		pf->setend(vec2(clickposx, clickposy));
		pf->calculate();
		flowspent = pf->getusedtime();

		for (auto& [x, y, num] : entitylist) {
			if (selected[num]) {
				//destlist[num] = {(float)clickposx, (float)clickposy, num};
				//printf("%d 's direction changed to %d %d", (int)num, clickposx, clickposy);
				
				//astar
				//pf->setfunc(a);
				//pf->setstart(vec2(x, y));
				//pf->setend(vec2(clickposx, clickposy));
				//pf->calculate();
				////a->postsmooth();

				//auto t = pf->getResult();
				//cout << t.size() << " !!!";
				//cout << "astar spent: " << pf->getusedtime() << endl;
				//astartotal += pf->getusedtime();

				
 				/*vector<vec2>* path = new vector<vec2>(t);
				if (t.size() == 0) {
					entities->setpath((int)num, nullptr);
				}
				else {
					entities->setpath((int)num, path);
				}*/

				// TODO: jps 최적화... 
				pf->setfunc(j);
				pf->setstart(vec2(x, y));
				pf->setend(vec2(clickposx, clickposy));
				j->changemod(false);
				pf->calculate();
				//j->postsmooth();

				auto jpsres = pf->getResult();
				//cout << jpsres.size() << " !!!";
				//cout << "jps+ origin spent: " << pf->getusedtime() << endl;
				jpstotal += pf->getusedtime();

				pf->setfunc(j);
				pf->setstart(vec2(x, y));
				pf->setend(vec2(clickposx, clickposy));
				j->changemod(true);
				pf->calculate();
				//j->postsmooth();

				auto jpsgoalres = pf->getResult();
				vector<vec2>* path = new vector<vec2>(jpsgoalres);

				//cout << jpsgoalres.size() << " !!!";
				
				
				//cout << "jps+ goal spent: " << pf->getusedtime() << endl;
				jpsgoal += pf->getusedtime();
				
				if (!mode_status[flowmode])
				{
					if (jpsgoalres.size() == 0) {
						entities->setpath((int)num, nullptr);
					}
					else {
						entities->setpath((int)num, path);
						destlist[num] = { path->at(0).x , path->at(0).y, num };
					}
				}
				else {
					entities->clearvelacc();
					entities->reached[num] = false;
					vector<vec2> temp = f->getpath(x, y);
					vector<vec2> *path = new vector<vec2>(temp);

					if (temp.size() == 0) {
						entities->setpath((int)num, nullptr);
					}
					else {
						entities->setpath((int)num, path);
						
					}
					//entities->setpath((int)num, )
					destlist[num] = { (float)clickposx,(float)clickposy, num };
				}

			}

		}

		textmanager->addText(0, "astar     spent: " + to_string(astartotal), vec4(0, 1, 1, 1), vec2(800 * zoomy, 950 * zoomx), 0.3 * zoomx);
		textmanager->addText(1, "jps+      spent: " + to_string(jpstotal), vec4(0, 1, 1, 1), vec2(800 * zoomy, 975 * zoomx), 0.3 * zoomx);
		textmanager->addText(2, "jps+ goal spent: " + to_string(jpsgoal), vec4(0, 1, 1, 1), vec2(800 * zoomy, 1000 * zoomx), 0.3 * zoomx);
		textmanager->addText(3, "flowfield spent: " + to_string(flowspent), vec4(0, 1, 1, 1), vec2(800 * zoomy, 925 * zoomx), 0.3 * zoomx);
		cout << "jps   : " << jpstotal << endl;
		cout << "jps + goal : " << jpsgoal << endl;
		cout << "astar : " << astartotal << endl;
		cout << "flowfield:" << flowspent << endl;
		/*for (auto &[x, y, num] : destlist) {
			cout << x << " " <<  y << " " << num << endl;
		}
		cout << endl;
		for (auto& [x, y, num] : entitylist) {
			cout << x << " " << y << " " << num << endl;
		}*/
	}
	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
		cout << "rt rel   x : " << mousex << "y : " << mousey << endl;
		mouse_status[rightmouse] = RELEASE;
	}
}

void makeEntities() {
	/*srand(time(nullptr));
	for (int i = 0; i < 100; i++) {
		int x = rand() % 1024;
		int y = rand() % 1024;
		entitylist.push_back({ (float)x, (float)y, (float)i });
	}*/

	/*
	entitylist.push_back({ 330, 200 , 0 });
	entitylist.push_back({ 330, 400 , 1 });
	entitylist.push_back({ 330, 600 , 2 });
	entitylist.push_back({ 550, 200 , 3 });
	entitylist.push_back({ 640, 400 , 4 });
	entitylist.push_back({ 240, 100 , 5 });
	entitylist.push_back({ 621, 700 , 6 });
	entitylist.push_back({ 246, 600 , 7 });
	entitylist.push_back({ 641, 500 , 8 });
	entitylist.push_back({ 244, 400 , 9 });
	entitylist.push_back({ 110, 200 , 10 });
	entitylist.push_back({ 830, 400 , 11 });
	entitylist.push_back({ 930, 500 , 12 });
	entitylist.push_back({ 650, 200 , 13 });
	entitylist.push_back({ 340, 470 , 14 });
	entitylist.push_back({ 140, 100 , 15 });
	entitylist.push_back({ 321, 690 , 16 });
	entitylist.push_back({ 216, 604 , 17 });
	entitylist.push_back({ 621, 500 , 18 });
	entitylist.push_back({ 240, 430 , 19 });
	entitylist.push_back({ 123, 345 , 20 });*/

	int e = 36;
	for (int i = 0; i < e; i++) {
		int x = 250 + 150 * cos(radians((float)i * 10));
		int y = 250 + 150 * sin(radians((float)i * 10));
		entitylist.push_back({ (float)x, (float)y, (float)i });

	}

	destlist = entitylist;
}