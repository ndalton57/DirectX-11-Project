#pragma once
#include "stdafx.h"

class RenderContext
{
	VertexShader* m_d3dVertexShader = nullptr;
	PixelShader* m_d3dPixelShader = nullptr;
	HullShader* m_d3dHullShader = nullptr;
	DomainShader* m_d3dDomainShader = nullptr;
	GeometryShader* m_d3dGeometryShader = nullptr;

public:
	RenderContext() = default;
	~RenderContext() = default;

	RenderContext(VertexShader* d3dVertexShader,
				  PixelShader* d3dPixelShader,
				  HullShader* d3dHullShader = nullptr,
				  DomainShader* d3dDomainShader = nullptr,
				  GeometryShader* d3dGeometryShader = nullptr);

	bool operator==(RenderContext& cContext);

	void Apply();
	void Revert();

	// Accessors //
	VertexShader* GetVertexShader() { return m_d3dVertexShader; }
	PixelShader* GetPixelShader() { return m_d3dPixelShader; }
	HullShader* GetHullShader() { return m_d3dHullShader; }
	DomainShader* GetDomainShader() { return m_d3dDomainShader; }
	GeometryShader* GetGeometryShader() { return m_d3dGeometryShader; }

	// Mutators //
	void SetVertexShader(VertexShader* d3dVertexShader) { m_d3dVertexShader = d3dVertexShader; }
	void SetPixelShader(PixelShader* d3dPixelShader) { m_d3dPixelShader = d3dPixelShader; }
	void SetHullShader(HullShader* d3dHullShader) { m_d3dHullShader = d3dHullShader; }
	void SetDomainShader(DomainShader* d3dDomainShader) { m_d3dDomainShader = d3dDomainShader; }
	void SetGeometryShader(GeometryShader* d3dGeometryShader) { m_d3dGeometryShader = d3dGeometryShader; }
};