#pragma once
#include "Renderer.h"

struct ShaderMacro
{
	string chMacroName;
	uint nMaxValue;
};

template <typename ID3D11Shader>
class Shader
{
	uint m_nTotalShaders = 0;
	uint m_nCurrentIndex = 0; // Currently selected shader
	vector<ShaderMacro> m_tMacroArr; // Macro defines

	ID3D11Shader** m_d3dShaderArr = nullptr;
	ID3D11InputLayout* m_d3dInputLayout = nullptr; // For vertex shaders

	// Helper Functions //
	uint FlattenIndex(vector<uint> nMacroValues, uint nIndex = 0, uint nDepth = 0); // Recursive // Flattens a multi-dimensional array into one dimension

public:
	Shader(uint nTotalShaders, ID3D11Shader** d3dShaderArr, vector<ShaderMacro> tMacroArr = vector<ShaderMacro>(), ID3D11InputLayout* d3dInputLayout = nullptr);
	~Shader();

	void operator=(Shader<ID3D11Shader>&) = delete;
	Shader(Shader<ID3D11Shader>&) = delete;

	operator ID3D11Shader*() { return m_d3dShaderArr[m_nCurrentIndex]; }
	void Update(vector<uint> nMacroValues = std::vector<uint>());

	void SetInputLayout(ID3D11InputLayout* d3dInputLayout) { m_d3dInputLayout = d3dInputLayout; }
	ID3D11InputLayout* GetInputLayout() { return m_d3dInputLayout; }
};

template <typename ID3D11Shader>
Shader<ID3D11Shader>::Shader(uint nTotalShaders, ID3D11Shader** d3dShaderArr, vector<ShaderMacro> tMacroArr, ID3D11InputLayout* d3dInputLayout)
{
	m_nTotalShaders = nTotalShaders;
	m_d3dShaderArr = d3dShaderArr;
	m_tMacroArr = tMacroArr;
	m_d3dInputLayout = d3dInputLayout;
}

template <typename ID3D11Shader>
Shader<ID3D11Shader>::~Shader()
{
	ARRAY_RELEASE(m_d3dShaderArr, m_nTotalShaders);
	delete[] m_d3dShaderArr;
}

template <typename ID3D11Shader>
void Shader<ID3D11Shader>::Update(std::vector<uint> nMacroValues)
{
	if (nMacroValues.size() == m_tMacroArr.size())
		m_nCurrentIndex = FlattenIndex(nMacroValues);
	else
		assert(!"Passed in macro values are not compatible with current shader's macro set! Incompatibility: Too many values");

	if (m_nCurrentIndex >= m_nTotalShaders)
		assert(!"Passed in macro values are not compatible with current shader's macro set! Incompatibility: One or more values exceeds it's maximum");
}

// --- Helper Functions --- //
template <typename ID3D11Shader>
uint Shader<ID3D11Shader>::FlattenIndex(std::vector<uint> nMacroValues, uint nIndex, uint nDepth)
{
	if (m_tMacroArr.size() == 0) // Early out for normal shaders
		return 0;

	uint number = 1;
	for (uint i = nDepth; i < nMacroValues.size(); i++)
		(i == nDepth) ? (number *= nMacroValues[nDepth]) : (number *= (m_tMacroArr[i].nMaxValue + 1));

	nIndex += number;

	if (++nDepth == nMacroValues.size())
		return nIndex;
	else
		return FlattenIndex(nMacroValues, nIndex, nDepth);
}