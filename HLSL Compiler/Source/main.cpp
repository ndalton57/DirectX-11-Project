// HLSL Compiler //
#include "Compiler.h"

int gReturnCode = 0;

int main(int argc, char* argv[])
{
	Compiler comp;

	comp.ParseArguments(argc, argv);
	comp.Compile();

	return gReturnCode;
}