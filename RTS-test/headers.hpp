#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <GLM/ext.hpp>
#include <vector>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <map>
#include <array>
#include <bitset>
#include <queue>
#include <functional>
#include <chrono>
#include <filesystem>
#include <ft2build.h>
#include FT_FREETYPE_H


using namespace std;
using namespace glm; 

static const int fixedx = 1024;
static const int fixedy = 1024;

static const int sizex = 1024; //5;    //const
static const int sizey = 1024;// 9;

static const float zoomx = (float)sizex / fixedx;
static const float zoomy = (float)sizey / fixedy;

static const int cellsize = 1;
static const string mapfile = "map.txt";//"map4096.txt";

static volatile double nowtime;
static volatile double prevtime;

const int keys_len = 8;
const int mouse_len = 4;
const int mode_len = 3;

typedef enum keys : int {
	ESC, S, LCTRL, n1, n2, n3, n4, LALT
}keys;

typedef enum status : int {
	RELEASE, PRESS, NON, DRAGGING
}status;

typedef enum mousemotion : int {
	leftmouse, rightmouse
}mousemotion;

typedef enum mode : int {
	flowmode, pathmode, rvo
}mode;
namespace input {
	extern int mousex;
	extern int mousey;
	extern int clickposx;
	extern int clickposy;
	extern bool togglebbox;
	extern double timeold;
	extern double timenow;
	extern status keys_status[keys_len];
	extern status mouse_status[mouse_len];
	extern bool mode_status[mode_len];
}
namespace mapinfo {
	//extern int arr[sizey / cellsize][sizex / cellsize];
	extern vector<vector<int>> arr;
	extern vector<array<float, 3>> entitylist;
	extern vector<array<float, 3>> destlist;
	extern bitset<100> selected; //TODO: change this after
	extern int radius;
}