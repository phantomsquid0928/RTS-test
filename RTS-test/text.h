#pragma once
#include "shader/shader.h"

class text : public shader
{
public:
	text();
	void create();
	void render();
	void update();
	void addText(string text);
	void changeText(int idx, string text);
private:
	vector<string> texts;

};
