#include "Compiler.h"

void Compiler::ParseArguments(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
	{
		if (string(argv[i] + 1) == "Mode")
			(string(argv[++i]) == "Debug") ? m_bDebug = true : m_bDebug = false;
		else if (string(argv[i] + 1) == "ShaderModel")
			m_chShaderModel = argv[++i];
		else if (string(argv[i] + 1) == "EntryPoint")
			m_chEntryPoint = argv[++i];
		else if (string(argv[i] + 1) == "FilePath")
			m_chFilePath = argv[++i];
		else if (string(argv[i] + 1) == "Var")
			m_tMacroArr.push_back({ argv[++i], argv[++i] });
	}
}

void Compiler::Compile()
{
	GrabFileName(); // Reads file name from path

	// Calculate Number Of Variations //
	for (uint i = 0; i < m_tMacroArr.size(); i++)
		m_nVariations *= (stoi(m_tMacroArr[i].chMaxValue) + 1);

	m_chOutputArr.resize(m_nVariations);

	// Shader Creation //
	if (m_nVariations == 1) // Normal shader with no macro variables
		gReturnCode = FXC_CALL(GenerateCallString().c_str()); // Standard compilation
	else // Shader with macro variables
	{
		cout << "Shaders to compile: " << m_nVariations << endl << endl;

		CompileVariants(vector<uint>(m_tMacroArr.size())); // Recursive start

		cout << endl << "...Done!" << endl << "Total shader compiled: " << m_nTotalShaders << endl;
	}

	cout << endl;

	GenerateIncludeHeader();
}

// ------------------------- //
// --- Private Functions --- //
// ------------------------- //

void Compiler::CompileVariants(vector<uint> nMacroValues, uint nDepth)
{
	uint& nCurrent = nMacroValues[nDepth]; // Value that's being looped through this iteration
	uint nMax = stoi(m_tMacroArr[nDepth].chMaxValue); // Max value to loop to
	for (nCurrent; nCurrent <= nMax; nCurrent++)
	{
		if (nDepth == nMacroValues.size() - 1) // Check if we're on the deepest recurse // Last macro
			gReturnCode = FXC_CALL(GenerateCallString(nMacroValues).c_str()); // Compiles using current set of values
		else
			CompileVariants(nMacroValues, nDepth + 1); // Keep recursing
	}
}

void Compiler::GrabFileName()
{
	uint nNameStart = 0;
	uint nIndex = (uint)m_chFilePath.size() - 1; // Start at end
	while (m_chFilePath[nIndex] != '/' && m_chFilePath[nIndex] != '\'') // Traverse back until first folder change
		nNameStart = nIndex--;

	for (uint i = nNameStart; i < m_chFilePath.size(); i++) // Add file name
		m_chFileName += m_chFilePath[i];
}

string Compiler::GenerateCallString(vector<uint> nMacroValues)
{
	// Generate Output Name //
	m_chOutputName = m_chFileName; // Initialize as default
	for (uint i = 0; i < m_tMacroArr.size(); i++) // Append each variable and it's value
		m_chOutputName += '_' + m_tMacroArr[i].chMacroName + '_' + to_string(nMacroValues[i]);
	
	m_chOutputArr[m_nTotalShaders++] = m_chOutputName; // Storing for header generation

	// Generate Call String //
	string chShaderMacros; // FXC preprocessor definition arguments
	for (uint i = 0; i < m_tMacroArr.size(); i++)
		chShaderMacros += string("/D ") + '"' + m_tMacroArr[i].chMacroName + '=' + to_string(nMacroValues[i]) + '"' + ' ';
	
	return "fxc.exe /T " + m_chShaderModel +
		   ((m_bDebug) ? " /Zi" : "") +
		   " /E " + '"' + m_chEntryPoint + '"' +
		   ((m_bDebug) ? " /Od" : "") +
		   " /Vn " + '"' + m_chOutputName + '"' +
		   " /Fo " + '"' + COMPILED_SHADER_OUTPUT + m_chFileName + '/' + ((m_bDebug) ? "Debug/" : "Release/") + m_chOutputName + ".cso\"" +
		   " /Fh " + '"' + COMPILED_SHADER_OUTPUT + m_chFileName + '/' + ((m_bDebug) ? "Debug/" : "Release/") + m_chOutputName + ".csh\"" +
		   " /nologo " + chShaderMacros +
		   m_chFilePath + ".hlsl";
}

void Compiler::GenerateIncludeHeader()
{
	ofstream fout;
	fout.open("Source/Shaders/Includes/" + m_chFileName + ".h");

	if (fout.is_open())
	{
		// Generate Includes //
		fout << "#pragma once" << endl << endl;
		fout << "// " << m_chFileName << " Includes //" << endl;
		fout << "#include \"../../Shader.h\"" << endl << endl;

		for (uint i = 0; i < m_chOutputArr.size(); i++)
			fout << "#include \"../Compiled Shaders/" << m_chFileName << '/' << ((m_bDebug) ? "Debug/" : "Release/") << m_chOutputArr[i] + ".csh\"" << endl;
		

		// Generate Load Function //
		string chShaderType;
		if (m_chShaderModel[0] == 'v')
			chShaderType = "Vertex";
		else if (m_chShaderModel[0] == 'p')
			chShaderType = "Pixel";
		else if (m_chShaderModel[0] == 'h')
			chShaderType = "Hull";
		else if (m_chShaderModel[0] == 'd')
			chShaderType = "Domain";
		else if (m_chShaderModel[0] == 'g')
			chShaderType == "Geometry";
		else if (m_chShaderModel[0] == 'c')
			chShaderType == "Compute";

		fout << endl << "// " << m_chFileName << " Load Function //" << endl;

		if (chShaderType == "Vertex")
			fout << "void " << "Load" << m_chFileName << "(Shader<ID3D11" << chShaderType << "Shader>*& d3dShader, D3D11_INPUT_ELEMENT_DESC* d3dElementDesc, uint nNumElements)" << endl;
		else
			fout << "void " << "Load" << m_chFileName << "(Shader<ID3D11" << chShaderType << "Shader>*& d3dShader)" << endl;

		fout << '{' << endl;

		fout << "\t" << "ID3D11" << chShaderType << "Shader** d3dShaderArr = new ID3D11" << chShaderType << "Shader*[" << m_nVariations << "];" << endl << endl;

		for (uint i = 0; i < m_chOutputArr.size(); i++)
			fout << "\t" << "GRAPHICS->GetDevice()->Create" << chShaderType << "Shader(" << m_chOutputArr[i] << ", sizeof(" << m_chOutputArr[i] << "), nullptr, &d3dShaderArr[" << i << "]);" << endl;

		if (chShaderType == "Vertex")
		{
			fout << endl;
			fout << '\t' << "ID3D11InputLayout* d3dInputLayout = nullptr;" << endl;
			fout << '\t' << "HRESULT hResult = GRAPHICS->GetDevice()->CreateInputLayout(d3dElementDesc, nNumElements, " << m_chOutputArr[0] << ", sizeof(" << m_chOutputArr[0] << "), &d3dInputLayout);" << endl;
			fout << '\t' << "assert(\"Input layout create failed!\" && SUCCEEDED(hResult));" << endl;
			fout << '\t' << "DX_NAME(d3dInputLayout, \"" << m_chFileName << " Input Layout\");" << endl;
		}

		fout << endl << "\t" << "d3dShader = new Shader<ID3D11" << chShaderType << "Shader>(" << m_nTotalShaders << ", d3dShaderArr";

		if (m_tMacroArr.size() > 0)
		{
			fout << ", { ";

			for (uint i = 0; i < m_tMacroArr.size(); i++)
				fout << "{ " << '"' << m_tMacroArr[i].chMacroName << '"' << ", " << m_tMacroArr[i].chMaxValue << " }" << ((i != m_tMacroArr.size() - 1) ? (", ") : (""));

			fout << " }";
		}

		if (chShaderType == "Vertex" && m_tMacroArr.size() == 0)
			fout << ", vector<ShaderMacro>(), d3dInputLayout";
		else if (chShaderType == "Vertex" && m_tMacroArr.size() > 0)
			fout << ", d3dInputLayout";

		fout << ");" << endl;


		fout << '}';
	}

	fout.close();
}