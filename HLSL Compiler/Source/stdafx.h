#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

typedef unsigned int uint;

#define FXC_CALL(chCallString) system(chCallString)
#define COMPILED_SHADER_OUTPUT "Source/Shaders/Compiled Shaders/" // Change as needed

extern int gReturnCode;