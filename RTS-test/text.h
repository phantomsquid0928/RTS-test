#pragma once
#include "shader/shader.h"

struct Character {
	GLuint TextureID;
	ivec2 Size;
	ivec2 Bearing;
	GLuint Advance;
};
struct label {
	string textline;
	vec4 color;
	vec2 location;
	float scale;
	label(string textline, vec4 color = vec4(0, 0, 0, 0), vec2 location = vec2(0, 0), float scale = 1) : textline(textline), color(color), location(location), scale(scale)
	{};
};


class text : public shader
{
public:
	text();
	text(string fontpath);
	void addText(int idx, string text, vec4 color, vec2 location, float scale);
	void create();
	void render();
	void update();
	void renderOne(string text, float x, float y, float scale, vec3 color);
	map<char, Character> getLoadedChars();
private:
	vector<label> texts;
	vector<float> vertices;
	string fontpath;
	FT_Library ft;
	FT_Face face;
	map<char, Character> Characters;
};

