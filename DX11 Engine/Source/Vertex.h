#pragma once
#include "stdafx.h"

struct MeshInstance;

struct Vertex3D
{
	float m_fPosition[4];
	float m_fTexCoords[4];
	float m_fNormal[4];
	float m_fTangent[4];
};

struct Mesh
{
	// Instance Data //
	uint m_nInstanceCount = 0; // How many RenderObject's are using this mesh
	uint m_nIndexStart = 0; // Starting index location
	uint m_nVertexStart = 0; // Starting vertex location

	std::vector<uint> m_nIndexArr;
	std::vector<Vertex3D> m_cVertexArr;
};