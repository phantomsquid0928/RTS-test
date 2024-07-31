#include "text.h"

text::text(string fontpath) {
	this->fontpath = fontpath;
	

	vcode = readFile("shader/text_vertex.vs"); //mod
	fcode = readFile("shader/text_fragment.fs");
	ft = NULL;
	face = NULL;
	this->shaderProgram = createShaderProgram(vcode.c_str(), fcode.c_str());
	cout << "text shader : " << shaderProgram << endl;

	// FreeType
	// --------
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}

	// find path to font
	std::string font_name = "shader/arial.ttf";

	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;
	}
	else {
		// set size to load glyphs as
		FT_Set_Pixel_Sizes(face, 0, 48);

		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 characters of ASCII set
		for (unsigned char c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}
void text::addText(int idx, string text, vec4 color, vec2 location, float scale) {
	if (texts.size() <= idx) texts.push_back(label(text, color, location, scale));
	else texts[idx] = label(text, color, location, scale);
	//texts[idx] = label(text, color, location, scale);
	update();
}
void text::create() {
	
}
void text::renderOne(string text, float x, float y, float scale, vec3 color) {
	// activate corresponding render state	
	glUseProgram(this->shaderProgram);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "textColor"), color.x, color.y, color.z);
	mat4 projection = ortho(0.0f, (float)sizey, 0.0f, (float)sizex);
	glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}
void text::render() {
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	int i = 0;
	for (auto& text : texts) {

		
		float x = text.location.x;
		glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), text.color.x, text.color.y, text.color.z); //TODO: may cause problem
		
		mat4 projection = ortho(0.0f, (float)sizey, 0.0f, (float)sizex);
		glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		for (auto& c : text.textline) {
			Character ch = Characters[c];
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			glDrawArrays(GL_TRIANGLES, i * 6, 6);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			x += (ch.Advance >> 6) * text.scale;
			i++;
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		//i++;
	}
	glBindVertexArray(0);

	glUseProgram(0);
}
void text::update() {
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	vertices.clear();

	for (auto& text : texts) {
		float x = text.location.x;
		for (auto& c : text.textline) {
			Character ch = Characters[c];

			float xpos = x + ch.Bearing.x * text.scale;
			float ypos = text.location.y - (ch.Size.y - ch.Bearing.y) * text.scale;

			float w = ch.Size.x * text.scale;
			float h = ch.Size.y * text.scale;

			vertices.insert(vertices.end(), {
				xpos, ypos + h, 0.0f, 0.0f,
				xpos, ypos, 0.0f, 1.0f ,
				xpos + w, ypos, 1.0f, 1.0f ,

				xpos, ypos + h, 0.0f, 0.0f ,
				xpos + w, ypos, 1.0f, 1.0f ,
				xpos + w, ypos + h, 1.0f, 0.0f });

			x += (ch.Advance >> 6) * text.scale;
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

map<char, Character> text::getLoadedChars() {
	return this->Characters;
}