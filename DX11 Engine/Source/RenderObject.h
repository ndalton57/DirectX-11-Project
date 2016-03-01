#pragma once
#include "stdafx.h"
#include "Vertex.h"
#include "RenderContext.h"
#include "Shader.h"

#define DIFFUSE_ERROR_PATH L"Assets/DiffuseError.dds"
#define NORMAL_ERROR_PATH L"Assets/NormalError.dds"
#define SPECULAR_ERROR_PATH L"Assets/SpecularError.dds"

enum MapType { Diffuse, Normal, Specular };

struct ShaderBind
{
	PixelShader* d3dPixelShader = nullptr;
	vector<uint*> nValues;
};

class RenderObject
{
	// Render Options //
	bool m_bActive = true; // For manual cull
	bool m_bCulled = false; // For frustum culling
	bool m_bEdgeGlow = false;
	bool m_bTransparent = false;
	float4 m_fColorOverride = { -1.0f, -1.0f, -1.0f, -1.0f };

	Mesh* m_cMesh = nullptr;
	RenderContext m_cContext; // For context sorting
	float4x4 m_fTransformMatrix;

	// Textures //
	ID3D11ShaderResourceView* m_d3dDiffuseMap = nullptr;
	ID3D11ShaderResourceView* m_d3dNormalMap = nullptr;
	ID3D11ShaderResourceView* m_d3dSpecularMap = nullptr;

	void MapBuffers();
	void SetTexture(ID3D11ShaderResourceView*& d3dTexture, const wchar_t* chFilePath, MapType tMap);

public:
	RenderObject() = default;
	~RenderObject() = default;

	RenderObject(Mesh* cMesh, RenderContext cContext, const wchar_t* chDiffuseMapPath = nullptr, const wchar_t* chNormalMapPath = nullptr, const wchar_t* chSpecularMapPath = nullptr);
	
	void Render(bool bContextOverride = false);

	void SetDiffuseMap(const wchar_t* chFilePath) { SetTexture(m_d3dDiffuseMap, chFilePath, Diffuse); }
	void SetDiffuseMap(ID3D11ShaderResourceView* d3dDiffuseMap) { m_d3dDiffuseMap = d3dDiffuseMap; }

	void SetNormalMap(const wchar_t* chFilePath) { SetTexture(m_d3dNormalMap, chFilePath, Normal); }
	void SetNormalMap(ID3D11ShaderResourceView* d3dNormalMap) { m_d3dNormalMap = d3dNormalMap; }

	void SetSpecularMap(const wchar_t* chFilePath) { SetTexture(m_d3dSpecularMap, chFilePath, Specular); }
	void SetSpecularMap(ID3D11ShaderResourceView* d3dSpecularMap) { m_d3dSpecularMap = d3dSpecularMap; }

	void CalculateTangents();

	// Accessors //
	bool& GetActive() { return m_bActive; }
	bool& GetCulled() { return m_bCulled; }
	bool& GetTransparent() { return m_bTransparent; }
	float4& GetColorOverride() { return m_fColorOverride; }

	Mesh* GetMesh() { return m_cMesh; }
	RenderContext& GetContext() { return m_cContext; }
	XMFLOAT4X4& GetTransformMatrix() { return m_fTransformMatrix; }

	ID3D11ShaderResourceView* GetDiffuseMap() { return m_d3dDiffuseMap; }
	ID3D11ShaderResourceView* GetNormalMap() { return m_d3dNormalMap; }
	ID3D11ShaderResourceView* GetSpecularMap() { return m_d3dSpecularMap; }
};