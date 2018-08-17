// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
// STL library
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <chrono>
#include <thread>
#include <memory>

using namespace std;

#pragma comment(lib,"opengl32")

/*
The OpenGL Extension Wrangler Library
http://glew.sourceforge.net/
*/
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/wglew.h>
#ifdef NDEBUG
#pragma comment(lib,"libglew32")
#else
#pragma comment(lib,"libglew32d")
#endif


/*
GLFW - An OpenGL library
http://www.glfw.org/
*/
#include <GLFW/glfw3.h>
#ifdef NDEBUG
#pragma comment(lib,"glfw3")
#else
#pragma comment(lib,"glfw3d")
#endif

/*
OpenGL Mathematics
http://glm.g-truc.net/
*/
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
The FreeType Project
https://www.freetype.org/
*/
#include <ft2build.h>
#include FT_FREETYPE_H
#ifdef NDEBUG
#pragma comment(lib,"freetype")
#else
#pragma comment(lib,"freetyped")
#endif

using uint = unsigned int;

const double pi = 3.141592653589793238462;