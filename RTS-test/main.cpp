#include "headers.hpp"
#include "mouse.h"
#include "shader/shader.h"
#include "block.h"
#include "entity.h"

int loadMap();
int saveMap();



void keyhandler(GLFWwindow* window, int key, int code, int action, int mode);
void mousehandler(GLFWwindow* window, double xpos, double ypos);
void clickhandler(GLFWwindow* window, int button, int action, int mods);


void tick();
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
	status mouse_status[sizeof(mousemotion)];
}
namespace mapinfo {
	int arr[sizey / cellsize][sizex / cellsize] = { 0 };
}

vector<array<int, 2>> walllist;

//vector<entity> entitylist;

using namespace input;

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

	glfwSetKeyCallback(window, /**function */keyhandler);
	glfwSetMouseButtonCallback(window, clickhandler);
	glfwSetCursorPosCallback(window, mousehandler);

	glewInit();

	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	
	glViewport(0, 0, bufferWidth, bufferHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(0.0, bufferWidth, bufferHeight, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	mouse m = mouse();
	m.render();


	loadMap();
	makeBlocks();
	makeEntities();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	block walls;
	walls.setPositions(walllist);

	entity a;
	entity b;
	entity c;


	

	do {
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		m.update();

		walls.render();

		
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	//saveMap();
	glfwDestroyWindow(window);
	glfwTerminate();
}

void tick() {


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
				mapinfo::arr[idy][idx] = i - '0';
			}
			else {
				switch (i) {
				case 'w':
					mapinfo::arr[idy][idx] = -1;
					break;
				default:
					mapinfo::arr[idy][idx] = 0;
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
			if (mapinfo::arr[i][j] < 0) { // wall
				ofs << "w";
			}
			else {
				ofs << mapinfo::arr[i][j];
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
		cout << "lctrl pressed" << endl;
		keys_status[LCTRL] = PRESS;
	}
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
		cout << "lctrl released" << endl;
		keys_status[LCTRL] = RELEASE;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		cout << "ESC pressed" << endl;
		keys_status[ESC] = PRESS;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		cout << "ESC released" << endl;
		keys_status[ESC] = RELEASE;
	}
}
void mousehandler(GLFWwindow* window, double xpos, double ypos) { //움직일때만 콜백이구만
	mousex = xpos;
	mousey = ypos;
	//cout << xpos << ypos << endl;
	//TODO: mouse escape? <
	if (mouse_status[leftmouse] == PRESS || mouse_status[leftmouse] == DRAGGING) { //drag // works!
		cout << "is dragging, " << mousex << " , " << mousey << " ~ " << clickposx << ", " << clickposy << endl;
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
		if (mouse_status[leftmouse] == DRAGGING) {
			//select units
		}
		mouse_status[leftmouse] = RELEASE;
	}
	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
		clickposx = mousex;
		clickposy = mousey; //depr
		cout << "rt click x : " << mousex << "y : " << mousey << endl;
		mouse_status[rightmouse] = PRESS;
	}
	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
		cout << "rt rel   x : " << mousex << "y : " << mousey << endl;
		mouse_status[rightmouse] = RELEASE;
	}
}
// class vector2 : public vec2 {
// 
// };

void makeBlocks() {
	for (int i = 0; i < sizey; i++) {
		for (int j = 0; j < sizex; j++) {
			if (mapinfo::arr[i][j] != 0) {
				walllist.push_back({ i, j });
			}
		}
	}
}

void makeEntities() {
	
}