#ifndef DEF_SHADER_HPP
#define DEF_SHADER_HPP

/*
	OpenGL shader management class
*/
#include "stdafx.h"

enum class ShaderType {
	vertex, fragment
};

// the length of warning buffer
const unsigned int char_info_length = 2048;

class ShaderManager {
public:
	ShaderManager(ShaderType _type) :type{ _type }{}
	ShaderManager(ShaderType _type, const string &_shader_text, const string &_comment) :
		type{ _type }, shader_text{ _shader_text }, comment{ _comment } {}

	const string &get_shader_text() const { return shader_text; }
	const string &get_comment() const { return comment; }
	void set_shader_text(const string &_shader_text) { shader_text = _shader_text; }
	void set_comment(const string &_comment) { comment = _comment; }

	GLuint get_shader_ptr() {
		// determine shader type
		decltype(GL_VERTEX_SHADER) GLShaderType;
		if (type == ShaderType::fragment) {
			GLShaderType = GL_FRAGMENT_SHADER;
		}
		else if (type == ShaderType::vertex) {
			GLShaderType = GL_VERTEX_SHADER;
		}
		else {
			cerr << "invalid shader type";
			throw runtime_error{ "invalid shader type" };
		}

		GLuint shader_ptr;
		shader_ptr = glCreateShader(GLShaderType);

		// pointer cast
		const char* text_ptr = shader_text.c_str();
		glShaderSource(shader_ptr, 1, &text_ptr, NULL);
		glCompileShader(shader_ptr);

		// check status
		GLint success_flag;
		GLchar info_string[char_info_length];
		glGetShaderiv(shader_ptr, GL_COMPILE_STATUS, &success_flag);
		if (!success_flag) {
			glGetShaderInfoLog(shader_ptr, char_info_length, NULL, info_string);
			cerr << "shader compilation failed\n" << info_string;
			throw runtime_error("shader compilation failure");
		}

		cout << comment<<" shader successfully compiled.\n";
		return shader_ptr;
	}
	ShaderType get_type() const { return type; }

	
protected:
	ShaderType type;
	string shader_text,comment;
};

class ShaderProgramManager {
public:
	ShaderProgramManager(GLuint _vertex_shader_ptr, GLuint _fragment_shader_ptr,const string &_comment) :
		vertex_shader_ptr{ _vertex_shader_ptr }, fragment_shader_ptr{ _fragment_shader_ptr }, comment{_comment} {
		link_program();
	}
	ShaderProgramManager(const string &vertex_shader_text, const string &fragment_shader_text,const string &_comment) :
		vertex_shader_ptr{ ShaderManager{ ShaderType::vertex, vertex_shader_text,_comment + string{" vertex"} }.get_shader_ptr() },
		fragment_shader_ptr{ ShaderManager{ ShaderType::fragment,fragment_shader_text,_comment + string{" fragment"} }.get_shader_ptr() },
		comment{ _comment } {
		link_program();
	}

	// link program is called automatically in contructors
	void link_program() {
		program_ptr = glCreateProgram();
		glAttachShader(program_ptr, vertex_shader_ptr);
		glAttachShader(program_ptr, fragment_shader_ptr);
		glLinkProgram(program_ptr);

		GLint success_flag;
		glGetProgramiv(program_ptr, GL_LINK_STATUS, &success_flag);
		GLchar info_string[char_info_length];
		if (!success_flag) {
			glGetProgramInfoLog(program_ptr, char_info_length, NULL, info_string);
			cerr << "program linkage failed\n" << info_string;
			throw runtime_error{ "program linkage failure" };
		}
		cout << comment<<" shader program succesfully linked.\n";
	}

	void use_program() { glUseProgram(program_ptr); }

	GLuint get_program_ptr() { return program_ptr; }
protected:
	string comment;
	GLuint program_ptr;
	GLuint vertex_shader_ptr;
	GLuint fragment_shader_ptr;
};

#endif // !DEF_SHADER_HPP
