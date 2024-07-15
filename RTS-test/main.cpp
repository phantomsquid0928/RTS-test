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
	status keys_status[sizeof(keys)];
	int mousex;
	int mousey;
	int clickposx;
	int clickposy;
	double timeold;
	double timenow;
	double recenttime;
	status mouse_status[sizeof(mousemotion)];
}
namespace mapinfo {
	vector<vector<int>> arr(sizey / cellsize, vector<int> (sizex / cellsize, 0));
	//int arr[sizey / cellsize][sizex / cellsize] = { 0 };
	vector<array<int, 3>> blocklist;
	vector<array<float, 3>> entitylist;
	vector<array<float, 3>> destlist;
	bitset<100> selected; //TODO: change this after
}

pathfinders *pf;
astarpath *a;
jpspath* j;
path* p;
entity *entities;
wall* walls;

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

	window = glfwCreateWindow(sizex, sizey, "____", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	//callbacks

	//set pf method as astar
	pf = new pathfinders();
	j = new jpspath();
	a = new astarpath(vec2(0, 0), vec2(0, 0));
	pf->setfunc(a);
	//pf->setfunc(j);

	

	glfwSetKeyCallback(window,	keyhandler);
	glfwSetMouseButtonCallback(window, clickhandler);
	glfwSetCursorPosCallback(window, mousehandler);

	glewInit();

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	
	glViewport(0, 0, bufferWidth, bufferHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(0.0, bufferWidth, 0.0, bufferHeight, -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	mouse m = mouse();
	m.render();


	loadMap();
	//loadEntities();

	//makeBlocks();
	makeEntities();
	selected.reset();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	walls = new wall();
	walls->create();

	entities = new entity(entitylist);
	entities->create();
	p = new path();
	p->create();

	recenttime = 0;

	//
	destlist = entitylist;

	timeold = glfwGetTime();
	int count = 0;
	do {
		
		glClear(GL_COLOR_BUFFER_BIT);
		//cout << (double)count / (timenow - timeold) << endl; //fps

		tick(&count);
		entities->render();
		
		//entities->
		int i = 0;

		
		walls->render();
		walls->update();
		m.update();

		glfwSwapBuffers(window);
		glfwPollEvents();
		count++;
	} while (glfwGetKey(window, GLFW_KEY_DELETE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	//saveMap();
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
				arr[idy][idx] = i - '0';
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

	for (int i = 0; i < sizey / cellsize; i++) {
		for (int j = 0; j < sizex / cellsize; j++) {
			if (arr[i][j] < 0) { // wall
				ofs << "w";
			}
			else {
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
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
		cout << "lctrl released" << endl;
		keys_status[LCTRL] = RELEASE;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		cout << "ESC pressed, clear bits" << endl;
		keys_status[ESC] = PRESS;
		arr = vector<vector<int>> (sizey / cellsize, vector<int>(sizex / cellsize, 0));
		selected.reset();
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		cout << "ESC released" << endl;
		keys_status[ESC] = RELEASE;
	}
	if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
		cout << "ALT pressed, adding wall mode on" << endl;
		keys_status[LALT] = PRESS;
	}
	if (key == GLFW_KEY_LEFT_ALT && action == GLFW_RELEASE) {
		cout << "ALT released" << endl;
		keys_status[LALT] = RELEASE;
	}
}
void mousehandler(GLFWwindow* window, double xpos, double ypos) { //움직일때만 콜백이구만
	mousex = xpos;
	mousey = ypos;
	//cout << xpos << ypos << endl;
	//TODO: mouse escape? <
	if (mouse_status[leftmouse] == PRESS || mouse_status[leftmouse] == DRAGGING) { //drag // works!
		//cout << "is dragging, " << mousex << " , " << mousey << " ~ " << clickposx << ", " << clickposy << endl;
		mouse_status[leftmouse] = DRAGGING;
		//render rectangle, write this on other func area
	}
	else {
		mouse_status[leftmouse] == NON;
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

		for (auto& [x, y, num] : entitylist) {
			if (selected[num]) {
				//destlist[num] = {(float)clickposx, (float)clickposy, num};
				printf("%d 's direction changed to %d %d", (int)num, clickposx, clickposy);
				
				pf->setfunc(a);
				pf->setstart(vec2(x, y));
				pf->setend(vec2(clickposx, clickposy));
				pf->calculate();
				a->postsmooth();

				auto t = pf->getResult();
				cout << t.size() << " !!!";
				cout << "spent: " << pf->getusedtime() << endl;


				// TODO: jps 최적화... 
				/*pf->setfunc(j);
				pf->setstart(vec2(x, y));
				pf->setend(vec2(clickposx, clickposy));
				pf->calculate();*/

				/*auto jpsres = pf->getResult(); 
				cout << jpsres.size() << " !!!";
				cout << "spent: " << pf->getusedtime() << endl;*/

				vector<vec2>* path = new vector<vec2>(t);
				//vector<vec2>* path = new vector<vec2>(jpsres);
				/*for (auto i : t) {
					cout << i.x << ":" << i.y << endl;
				}*/

				if (t.size() == 0) {
					entities->setpath((int)num, nullptr);
				}
				else {
					entities->setpath((int)num, path);
					
				}

				/*if (jpsres.size() == 0) {
					entities->setpath((int)num, nullptr);
				}
				else {
					entities->setpath((int)num, path);
					destlist[num] = { path->at(0).x , path->at(0).y, num };
				}*/
				
				cout << endl;
			}

		}
		for (auto &[x, y, num] : destlist) {
			cout << x << " " <<  y << " " << num << endl;
		}
		cout << endl;
		for (auto& [x, y, num] : entitylist) {
			cout << x << " " << y << " " << num << endl;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
		cout << "rt rel   x : " << mousex << "y : " << mousey << endl;
		mouse_status[rightmouse] = RELEASE;
	}
}

void makeBlocks() {
	for (int i = 0; i < sizey; i++) {
		for (int j = 0; j < sizex; j++) {
			if (arr[i][j] != -1 && arr[i][j] != 0) {
				blocklist.push_back({ i, j });
			}
		}
	}
}

void makeEntities() {
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
	entitylist.push_back({ 123, 345 , 20 });
}