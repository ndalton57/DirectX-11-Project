#pragma once
#include "stdafx.h"

inline void CreateSwapChainDesc(uint nWindowWidth, uint nWindowHeight, uint nSampleCount, uint nSampleQuality, 
								DXGI_SWAP_CHAIN_DESC& d3dSwapChainDesc)
{
	ZeroMemory(&d3dSwapChainDesc, sizeof(d3dSwapChainDesc));
	d3dSwapChainDesc.BufferDesc.Width = nWindowWidth;
	d3dSwapChainDesc.BufferDesc.Height = nWindowHeight;
	d3dSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	d3dSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	d3dSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dSwapChainDesc.SampleDesc.Count = nSampleCount;
	d3dSwapChainDesc.SampleDesc.Quality = nSampleQuality;
	d3dSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	d3dSwapChainDesc.BufferCount = 1;
	d3dSwapChainDesc.OutputWindow = gWindow;
	d3dSwapChainDesc.Windowed = true;
	d3dSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	d3dSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
}

inline void CreateViewport(uint nWindowWidth, uint nWindowHeight, D3D11_VIEWPORT& d3dViewport)
{
	ZeroMemory(&d3dViewport, sizeof(d3dViewport));
	d3dViewport.Width = (float)nWindowWidth;
	d3dViewport.Height = (float)nWindowHeight;
	d3dViewport.MinDepth = 0;
	d3dViewport.MaxDepth = 1;
	d3dViewport.TopLeftX = 0;
	d3dViewport.TopLeftY = 0;
}

inline void CreateSamplerDesc(D3D11_FILTER d3dFilter, D3D11_TEXTURE_ADDRESS_MODE d3dAddressMode, 
							  D3D11_SAMPLER_DESC& d3dSamplerDesc)
{
	ZeroMemory(&d3dSamplerDesc, sizeof(d3dSamplerDesc));
	d3dSamplerDesc.Filter = d3dFilter;
	d3dSamplerDesc.AddressU = d3dAddressMode;
	d3dSamplerDesc.AddressV = d3dAddressMode;
	d3dSamplerDesc.AddressW = d3dAddressMode;
	d3dSamplerDesc.MipLODBias = NULL;
	d3dSamplerDesc.MaxAnisotropy = 1;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.BorderColor[0] = 1.0f;
	d3dSamplerDesc.BorderColor[1] = 1.0f;
	d3dSamplerDesc.BorderColor[2] = 1.0f;
	d3dSamplerDesc.BorderColor[3] = 1.0f;
	d3dSamplerDesc.MinLOD = -FLT_MAX;
	d3dSamplerDesc.MaxLOD = FLT_MAX;
}

inline void CreateTextureDesc(uint nWindowWidth, uint nWindowHeight, DXGI_FORMAT d3dFormat, uint nSampleCount, 
							  uint nSampleQuality, uint nBindFlags, uint nCPUFlags, uint nMiscFlags, 
							  D3D11_TEXTURE2D_DESC& d3dTextureDesc)
{
	ZeroMemory(&d3dTextureDesc, sizeof(d3dTextureDesc));
	d3dTextureDesc.Width = nWindowWidth;
	d3dTextureDesc.Height = nWindowHeight;
	d3dTextureDesc.MipLevels = 1;
	d3dTextureDesc.ArraySize = 1;
	d3dTextureDesc.Format = d3dFormat;
	d3dTextureDesc.SampleDesc.Count = nSampleCount;
	d3dTextureDesc.SampleDesc.Quality = nSampleQuality;
	d3dTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dTextureDesc.BindFlags = nBindFlags;
	d3dTextureDesc.CPUAccessFlags = nCPUFlags;
	d3dTextureDesc.MiscFlags = nMiscFlags;
}

inline void CreateDSVDesc(uint nFlags, DXGI_FORMAT d3dFormat, D3D11_DSV_DIMENSION d3dDimension, 
						  D3D11_DEPTH_STENCIL_VIEW_DESC& d3dDSVDesc)
{
	ZeroMemory(&d3dDSVDesc, sizeof(d3dDSVDesc));
	d3dDSVDesc.Flags = nFlags;
	d3dDSVDesc.Format = d3dFormat;
	d3dDSVDesc.ViewDimension = d3dDimension;
}

inline void CreateSRVDesc(DXGI_FORMAT d3dFormat, D3D11_SRV_DIMENSION d3dDimension, 
						  D3D11_SHADER_RESOURCE_VIEW_DESC& d3dSRVDesc)
{
	ZeroMemory(&d3dSRVDesc, sizeof(d3dSRVDesc));
	d3dSRVDesc.Format = d3dFormat;
	d3dSRVDesc.ViewDimension = d3dDimension;
	d3dSRVDesc.Texture2D.MipLevels = 1;
}

inline void CreateBlendDesc(D3D11_BLEND_DESC& d3dBlendDesc)
{
	ZeroMemory(&d3dBlendDesc, sizeof(d3dBlendDesc));
	for (uint i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
	{
		d3dBlendDesc.RenderTarget[i].BlendEnable = true;
		d3dBlendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		d3dBlendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		d3dBlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		d3dBlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		d3dBlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		d3dBlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		d3dBlendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
}

inline void CreateRasterizerDesc(D3D11_FILL_MODE d3dFillMode, D3D11_CULL_MODE d3dCullMode, bool bMultiSampling, 
								 D3D11_RASTERIZER_DESC& d3dRasterizerDesc)
{
	ZeroMemory(&d3dRasterizerDesc, sizeof(d3dRasterizerDesc));
	d3dRasterizerDesc.FillMode = d3dFillMode;
	d3dRasterizerDesc.CullMode = d3dCullMode;
	d3dRasterizerDesc.FrontCounterClockwise = false;
	d3dRasterizerDesc.DepthBias = NULL;
	d3dRasterizerDesc.DepthBiasClamp = NULL;
	d3dRasterizerDesc.SlopeScaledDepthBias = true;
	d3dRasterizerDesc.DepthClipEnable = false;
	d3dRasterizerDesc.ScissorEnable = false;
	d3dRasterizerDesc.MultisampleEnable = bMultiSampling;
	d3dRasterizerDesc.AntialiasedLineEnable = false;
}

inline void CreateBufferDesc(uint nByteWidth, D3D11_USAGE d3dUsage, uint nBindFlags, uint nCPUAccessFlags, 
							 uint nByteStride, D3D11_BUFFER_DESC& d3dBufferDesc)
{
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.ByteWidth = nByteWidth;
	d3dBufferDesc.Usage = d3dUsage;
	d3dBufferDesc.BindFlags = nBindFlags;
	d3dBufferDesc.CPUAccessFlags = nCPUAccessFlags;
	d3dBufferDesc.MiscFlags = NULL;
	d3dBufferDesc.StructureByteStride = nByteStride;
}

inline void CreateBufferResource(void* vSource, D3D11_SUBRESOURCE_DATA& d3dBufferResource)
{
	ZeroMemory(&d3dBufferResource, sizeof(d3dBufferResource));
	d3dBufferResource.pSysMem = vSource;
	d3dBufferResource.SysMemPitch = NULL;
	d3dBufferResource.SysMemSlicePitch = NULL;
}

inline void CreateInputElementDesc(LPCSTR chSemanticName, DXGI_FORMAT d3dFormat, D3D11_INPUT_ELEMENT_DESC& d3dElementDesc)
{
	ZeroMemory(&d3dElementDesc, sizeof(d3dElementDesc));
	d3dElementDesc.SemanticName = chSemanticName;
	d3dElementDesc.SemanticIndex = NULL;
	d3dElementDesc.Format = d3dFormat;
	d3dElementDesc.InputSlot = NULL;
	d3dElementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	d3dElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	d3dElementDesc.InstanceDataStepRate = NULL;
}