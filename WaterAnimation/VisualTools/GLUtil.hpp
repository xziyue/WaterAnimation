#pragma once
#include "stdafx.h"

inline void print_GL_version() {
	cout << "Open GL version " << glGetString(GL_VERSION) << "\n";
}