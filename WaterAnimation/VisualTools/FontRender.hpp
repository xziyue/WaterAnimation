#ifndef DEF_FONTRENDER_HPP
#define DEF_FONTRENDER_HPP

#include "stdafx.h"
#include "Shader.hpp"
#include "ShaderSource.h"

#include <map>

struct Glyph {
	GLuint texture_id;
	glm::ivec2 size;
	glm::ivec2 bearing;
	GLuint advance;
};

class FontLoader {
public:
	FontLoader(const string &_typeface, uint _size,float _line_spread = 1.5f) :
		typeface{ _typeface }, font_size{ _size }, line_spread{_line_spread} {
		initilize_lib();
	}
	FontLoader(const FontLoader &other) = delete;
	FontLoader(FontLoader &&other) = delete;

	float get_line_spread() const { return line_spread; }
	float set_line_spread(float _ls) { line_spread = _ls; }

	~FontLoader() {
		delete program;
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
protected:
	float line_spread;
	FT_Library ft;
	FT_Face face;
	uint font_size;
	string typeface;

	GLuint VAO, VBO;
	map<wchar_t, Glyph> library;
	// font shader program
	ShaderProgramManager* program;
private:
	void initilize_lib() {
		// enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// create shader
		string font_vertex_shader_text = font_vertex_shader_source;
		string font_fragment_shader_text = font_fragment_shader_source;
		program = new ShaderProgramManager{ font_vertex_shader_text,font_fragment_shader_text,string{ "font rendering" } };

		// generate buffer for text rendering
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL,
			GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		if (FT_Init_FreeType(&ft)) {
			cerr << "unable to initialize the FreeType library\n";
			throw runtime_error{ "initialization error" };
		}
		if (FT_New_Face(ft, typeface.c_str(), 0, &face)) {
			cerr << "unable to load font typeface " << typeface << "\n";
			throw runtime_error{ "font loading error" };
		}
		cout << "FreeType has loaded font typeface " << typeface << "\n";

		// zero indicates 'same as other'
		FT_Set_Pixel_Sizes(face, 0, font_size);
		for (wchar_t c = 0; c < 128; ++c) {
			load_font(c,false);
		}
		check_break_line();
	}

	void core_draw(const Glyph &glyph, GLfloat x, GLfloat y, GLfloat scale) {
		GLfloat xpos = x + glyph.bearing.x * scale;
		GLfloat ypos = y - (glyph.size.y - glyph.bearing.y) * scale;
		GLfloat w = glyph.size.x * scale;
		GLfloat h = glyph.size.y * scale;

		GLfloat vertices[6][4] = {
			{ xpos,ypos + h,0.0f,0.0f },
			{ xpos,ypos,0.0f,1.0f },
			{ xpos + w,ypos,1.0f,1.0f },

			{ xpos,ypos + h, 0.0f,0.0f },
			{ xpos + w,ypos,1.0f,1.0f },
			{ xpos + w, ypos + h,1.0f,0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, glyph.texture_id);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

	}
	void core_start_up() {
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);
	}
	void core_clean_up() {
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	//make sure break line has valid size
	void check_break_line() {
		auto iter = library.find((wchar_t)'\n');
		if (iter->second.size.y == 0) {
			auto iter1 = library.find((wchar_t)'A');
			iter->second.size = iter1->second.size;
		}
	}

	map<wchar_t, Glyph>::iterator core_find(wchar_t c) {
		auto liter = library.find(c);
		if (liter == library.end()) {
			cerr << "Character " << c << " is not included in the glyph library. It is automatically loaded.\n";
			load_font(c);
			liter = library.find(c);
		}
		return liter;
	}
public:
	void load_font(wchar_t target,bool force_load = false) {
		if(!force_load)
			if (library.find(target) != library.end())return;

		if (FT_Load_Char(face, target, FT_LOAD_RENDER)) {
			cerr << "unable to load character " << target << " (Unicode index " << target << ").\n";
			throw runtime_error{ "font loading failure" };
		}
		/*
		if ((face->glyph->bitmap.rows * face->glyph->bitmap.width) % 4 != 0) {
			cerr << "the shape(" << face->glyph->bitmap.width << ", " << face->glyph->bitmap.rows << ")"
				<< " of the bitmap of the character " << target << " (Unicode index " << (int)target << ") does not"
				<< " meet the alignment requirement of OpenGL textures.";
			throw runtime_error{ "texture error" };
		}*/
		GLuint texture_ptr;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &texture_ptr);
		glBindTexture(GL_TEXTURE_2D, texture_ptr);
		glTexImage2D(GL_TEXTURE_2D, 0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Glyph glyph{ texture_ptr,
			glm::ivec2{ face->glyph->bitmap.width,face->glyph->bitmap.rows},
			glm::ivec2{face->glyph->bitmap_left,face->glyph->bitmap_top},
			face->glyph->advance.x
		};
		library.insert(pair<wchar_t, Glyph>{target, glyph});
		glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
	}

	// x,y : render position; w,h : size of the window
	void render_font(const wstring &text, GLfloat x, GLfloat y, uint w,uint h, GLfloat scale,const glm::vec3 &color) {
		program->use_program();
		glm::mat4 projection = glm::ortho(0.0f, (float)w, 0.0f, (float)h);
		glUniform3f(glGetUniformLocation(program->get_program_ptr(), "text_color"), color.x, color.y, color.z);
		glUniformMatrix4fv(glGetUniformLocation(program->get_program_ptr(), "projection"),1,false,glm::value_ptr(projection));
		core_start_up();

		auto init_x = x;
		auto init_y = y;
		for (auto iter = text.begin(); iter != text.end(); ++iter) {
			if (*iter == (wchar_t)'\n') {
				x = init_x;
				auto space = library.find((wchar_t)'A');
				y -= ((float)space->second.size.y * line_spread);
				continue;
			}
			auto liter = core_find(*iter);
			Glyph &glyph = liter->second;
			core_draw(glyph,x,y,scale);
			x += (glyph.advance >> 6) * scale;
		}
		core_clean_up();
	}

	void render_shadowed_font(const wstring &text, GLfloat x, GLfloat y, uint w, uint h, 
		GLfloat _offset, const glm::vec3 &fore_ground, const glm::vec3 &back_ground) {

		program->use_program();
		glm::mat4 projection = glm::ortho(0.0f, (float)w, 0.0f, (float)h);
		auto color_loc = glGetUniformLocation(program->get_program_ptr(), "text_color");
		glUniformMatrix4fv(glGetUniformLocation(program->get_program_ptr(), "projection"), 1, false, glm::value_ptr(projection));
		core_start_up();

		auto init_x = x;
		auto init_y = y;
		auto offset = _offset * (GLfloat)font_size;

		glUniform3f(color_loc, back_ground.x, back_ground.y, back_ground.z);
		vector<map<wchar_t, Glyph>::iterator> found;
		found.reserve(text.length());

		for (auto iter = text.begin(); iter != text.end(); ++iter) {
			if (*iter == (wchar_t)'\n') {
				x = init_x;
				auto space = library.find((wchar_t)'\n');
				y -= ((float)space->second.size.y * line_spread);
				found.emplace_back(space);
				continue;
			}
			auto liter = core_find(*iter);
			Glyph &glyph = liter->second;
			core_draw(glyph, x + offset, y - offset, 1.0);
			x += (glyph.advance >> 6);
			found.emplace_back(liter);
		}

		x = init_x;
		y = init_y;
		glUniform3f(color_loc, fore_ground.x, fore_ground.y, fore_ground.z);

		for (const auto &liter : found) {
			if (liter->first == (wchar_t)'\n') {
				x = init_x;
				y -= ((float)liter->second.size.y * line_spread);
				continue;
			}
			Glyph &glyph = liter->second;
			core_draw(glyph, x, y, 1.0);
			x += (glyph.advance >> 6);
		}
		core_clean_up();
	}
};

#endif // !DEF_FONTRENDER_HPP
