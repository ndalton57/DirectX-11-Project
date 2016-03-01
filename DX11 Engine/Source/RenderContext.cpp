#include "RenderContext.h"

#include "Shader.h"
#include "Renderer.h"

RenderContext::RenderContext(VertexShader* d3dVertexShader,
							 PixelShader* d3dPixelShader,
							 HullShader* d3dHullShader,
							 DomainShader* d3dDomainShader,
							 GeometryShader* d3dGeometryShader)
{
	m_d3dVertexShader = d3dVertexShader;
	m_d3dPixelShader = d3dPixelShader;
	m_d3dHullShader = d3dHullShader;
	m_d3dDomainShader = d3dDomainShader;
	m_d3dGeometryShader = d3dGeometryShader;
}

bool RenderContext::operator==(RenderContext& cContext)
{
	return (m_d3dVertexShader == cContext.m_d3dVertexShader &&
			m_d3dPixelShader == cContext.m_d3dPixelShader &&
			m_d3dHullShader == cContext.m_d3dHullShader &&
			m_d3dDomainShader == cContext.m_d3dDomainShader &&
			m_d3dGeometryShader == cContext.m_d3dGeometryShader);
}

void RenderContext::Apply()
{
	ID3D11DeviceContext* d3dContext = GRAPHICS->GetContext();

	if (m_d3dVertexShader)
		d3dContext->VSSetShader(*m_d3dVertexShader, nullptr, 0);
	if (m_d3dPixelShader)
		d3dContext->PSSetShader(*m_d3dPixelShader, nullptr, 0);
	if (m_d3dHullShader)
		d3dContext->HSSetShader(*m_d3dHullShader, nullptr, 0);
	if (m_d3dDomainShader)
		d3dContext->DSSetShader(*m_d3dDomainShader, nullptr, 0);
	if (m_d3dGeometryShader)
		d3dContext->GSSetShader(*m_d3dGeometryShader, nullptr, 0);

	if (m_d3dVertexShader)
		d3dContext->IASetInputLayout(m_d3dVertexShader->GetInputLayout());
}

void RenderContext::Revert()
{
	ID3D11DeviceContext* d3dContext = GRAPHICS->GetContext();

	d3dContext->VSSetShader(nullptr, nullptr, 0);
	d3dContext->PSSetShader(nullptr, nullptr, 0);
	d3dContext->HSSetShader(nullptr, nullptr, 0);
	d3dContext->DSSetShader(nullptr, nullptr, 0);
	d3dContext->GSSetShader(nullptr, nullptr, 0);

	d3dContext->IASetInputLayout(nullptr);
}