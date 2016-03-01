#pragma once

struct ShaderMacro
{
	string chMacroName;
	string chMaxValue;
};

class Compiler
{
	// Compile Variables //
	bool m_bDebug = false;
	uint m_nVariations = 1;
	uint m_nTotalShaders = 0;

	// Ouput Variables //
	string m_chShaderModel;
	string m_chEntryPoint;
	string m_chFileName;
	string m_chOutputName;
	string m_chFilePath;

	vector<string> m_chOutputArr;
	vector<ShaderMacro> m_tMacroArr;

	// Helper Functions //
	void CompileVariants(vector<uint> nMacroValues, uint nDepth = 0);

	void GrabFileName();
	string GenerateCallString(vector<uint> nMacroValues = vector<uint>()); // Generate call string for fxc.exe call with necessary arguments
	void GenerateIncludeHeader();

public:
	Compiler() = default;
	~Compiler() = default;

	void ParseArguments(int argc, char* argv[]); // Loops through argument list and assigns values to proper variables
	void Compile();
};

