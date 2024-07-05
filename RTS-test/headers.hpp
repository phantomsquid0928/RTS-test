#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <array>

using namespace std;
using namespace glm;


static const int sizex = 1024;
static const int sizey = 1024;
static const int cellsize = 1;
static const string mapfile = "map.txt";

static volatile double nowtime;
static volatile double prevtime;

typedef enum keys : int {
	ESC, S, LCTRL, n1, n2, n3, n4
}keys;

typedef enum status : int {
	RELEASE, PRESS, NON, DRAGGING
}status;

typedef enum mousemotion : int {
	leftmouse, rightmouse
}mousemotion;
namespace input {
	extern status keys_status[sizeof(keys)];
	extern int mousex;
	extern int mousey;
	extern int clickposx;
	extern int clickposy;
	extern status mouse_status[sizeof(mousemotion)];
}
namespace mapinfo {
	extern int arr[sizey / cellsize][sizex / cellsize];

}