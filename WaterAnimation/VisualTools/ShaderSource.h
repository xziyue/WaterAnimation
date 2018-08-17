#pragma once

#include "stdafx.h"

/*

# A small piece of Python code that does the job

path = raw_input("enter file name\n")
str = ""
with open(path,'r') as infile:
	for line in infile:
	if line[-1] == '\n':line = line[:-1]
	temp = "\"" + line + "\\n\"\n"
	str += temp
print str

*/

const string default_vertex_shader_source =
"#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 _color;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(position, 1.0f);\n"
"color = _color;\n"
"}\n"
;

const string default_fragment_shader_source =
"#version 330 core\n"
"in vec3 _color;\n"
"out vec4 color;\n"
"void main(){\n"
"color = vec4(_color,1.0f);\n"
"}\n"
;

const string font_vertex_shader_source =
"#version 330 core\n"
"layout (location = 0) in vec4 vertex;\n"
"out vec2 tex_coor;\n"
"uniform mat4 projection;\n"
"void main(){\n"
"gl_Position = projection * vec4(vertex.xy,0.0,1.0);\n"
"tex_coor = vertex.zw;\n"
"}\n"
;

const string font_fragment_shader_source =
"#version 330 core\n"
"in vec2 tex_coor;\n"
"out vec4 color;\n"
"\n"
"uniform sampler2D text;\n"
"uniform vec3 text_color;\n"
"\n"
"void main(){\n"
"vec4 sampled = vec4(1.0,1.0,1.0,texture(text,tex_coor).r);\n"
"color = vec4(text_color,1.0) * sampled;\n"
"}\n"
;