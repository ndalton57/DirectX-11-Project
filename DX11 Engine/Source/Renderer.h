#pragma once
#include "stdafx.h"
#include "Vertex.h"

#include <unordered_map>

//Graphics Todo
//Change shader compilation to pre - compiled
//Fix mip - mapping
//Tangent calculation
//Correct resolution swapping
//Initialize and Reinitialize
//Post Process effects

// Forward Declarations //
class Camera;
class RenderObject;
class RenderNode;
struct PointLight;
struct DirectionalLight;

class Renderer
{
	// Debug Variables //
	uint m_nTotalObjects = 0;
	bool m_bDebugCamera = true;
	Camera* m_cDebugCamera = nullptr;

	// Render Options //
	float m_fClearColor[4];
	bool m_bFullScreen = false;
	uint m_nSampleCount;
	uint m_nSampleQuality;

	// Scene Data // - Might abstract out into camera class
	float m_fNearPlane = 0.1f;
	float m_fFarPlane = 25000.0f;

	float4x4 m_fViewMatrix;
	float4x4 m_fProjectionMatrix;

	// Render Data //
	RenderObject* m_cSkybox = nullptr;
	vector<RenderNode> m_cRenderArr; // List of objects to render // Renders first
	vector<RenderNode> m_cTransparentArr; // List of transparent objects to render // Renders second

	DirectionalLight* m_cDirectionalLight = nullptr;
	vector<PointLight*> m_cLightArr;

	vector<Mesh*> m_cMeshes; // List of meshes being used
	unordered_map<wstring, ID3D11ShaderResourceView*> m_d3dTextureList; // All textures

	Mesh* m_cScreenQuadMesh = nullptr;
	RenderObject* m_cScreenQuad = nullptr;

	bool m_bMeshUpdate = false; // Triggers if meshes have been added or removed, requiring a remap to the GPU

	// --- DirectX Variables --- //
	ID3D11Device* m_d3dDevice = nullptr;
	ID3D11DeviceContext* m_d3dContext = nullptr;
	IDXGISwapChain* m_d3dSwapChain = nullptr;

	ID3D11SamplerState* m_d3dSamplerState = nullptr;

	// Current Outputs //
	ID3D11RenderTargetView* m_d3dCurrentRTV = nullptr;
	ID3D11DepthStencilView* m_d3dCurrentDSV = nullptr;

	// Back Buffer //
	ID3D11Texture2D* m_d3dBackBuffer = nullptr;
	ID3D11RenderTargetView* m_d3dBackBufferRTV = nullptr;

	// Scene Texture // - Main scene gets rendered to this
	ID3D11Texture2D* m_d3dSceneTexture = nullptr; // Main scene
	ID3D11RenderTargetView* m_d3dSceneTextureRTV = nullptr; // To bind as render target
	ID3D11ShaderResourceView* m_d3dSceneTextureSRV = nullptr; // To bind as shader resource

	// Depth Buffer //
	ID3D11Texture2D* m_d3dDepthBuffer = nullptr; // Main depth buffer
	ID3D11DepthStencilView* m_d3dDepthBufferDSV = nullptr; // To bind as depth buffer
	ID3D11ShaderResourceView* m_d3dDepthBufferSRV = nullptr; // To bind as shader resource

	// Render States //
	ID3D11BlendState* m_d3dTransparentState = nullptr;
	ID3D11RasterizerState* m_d3dSolidState = nullptr;
	ID3D11RasterizerState* m_d3dWireframeState = nullptr;

	// Mesh Buffers //
	uint m_nIndexCount = 0;
	ID3D11Buffer* m_d3dIndexBuffer = nullptr;

	uint m_nVertexCount = 0;
	ID3D11Buffer* m_d3dVertexBuffer = nullptr;

	// Constant Buffers // - Vertex Shader
	ID3D11Buffer* m_d3dTransformBuffer = nullptr; // b0 // Transform matrix
	ID3D11Buffer* m_d3dViewBuffer = nullptr; // b1 // View matrix
	ID3D11Buffer* m_d3dProjectionBuffer = nullptr; // b2 // Projection matrix

	// Constant Buffers // - Pixel Shader
	ID3D11Buffer* m_d3dPropertyBuffer = nullptr; // b5 // Material properties
	ID3D11Buffer* m_d3dDLightBuffer = nullptr; // b6 // Directional light
	ID3D11Buffer* m_d3dLightBuffer = nullptr; // b7 // Light list
	ID3D11Buffer* m_d3dCameraBuffer = nullptr; // b8 // Camera Position

	// Shaders //
	VertexShader* m_d3dSkyboxVS = nullptr;
	VertexShader* m_d3dStandardVS = nullptr;
	VertexShader* m_d3dScreenVS = nullptr;

	PixelShader* m_d3dSkyboxPS = nullptr;
	PixelShader* m_d3dStandardPS = nullptr; // Variable // MAX_LIGHTS
	PixelShader* m_d3dScreenPS = nullptr;

	// Input Layouts //
	ID3D11InputLayout* m_d3dVertex2DLayout = nullptr;
	ID3D11InputLayout* m_d3dVertex3DLayout = nullptr;

	// Helper Functions //
	void MapBuffers();
	void RenderPass();
	void PostProcess();

	inline void Present() { m_d3dSwapChain->Present(0, 0); }

	void ClearScreen(ID3D11RenderTargetView* d3dRTV = nullptr) { if (!d3dRTV) d3dRTV = GetCurrentRTV(); m_d3dContext->ClearRenderTargetView(d3dRTV, m_fClearColor); }
	void ClearDepth(ID3D11DepthStencilView* d3dDSV = nullptr) { if (!d3dDSV) d3dDSV = GetCurrentDSV(); m_d3dContext->ClearDepthStencilView(d3dDSV, D3D11_CLEAR_DEPTH, 1.0f, 0); }

	void SetRenderTarget(ID3D11RenderTargetView* d3dRTV) { ID3D11DepthStencilView* d3dDSV = GetCurrentDSV(); m_d3dContext->OMSetRenderTargets(1, &d3dRTV, d3dDSV); m_d3dCurrentRTV = d3dRTV; }
	void SetDepthStencil(ID3D11DepthStencilView* d3dDSV) { ID3D11RenderTargetView* d3dRTV = GetCurrentRTV(); m_d3dContext->OMSetRenderTargets(1, &d3dRTV, d3dDSV); m_d3dCurrentDSV = d3dDSV; }

	void UnbindVertexShaderSRV(uint nSlot) { ID3D11ShaderResourceView* d3dBlankSRV = nullptr; m_d3dContext->VSSetShaderResources(nSlot, 1, &d3dBlankSRV); }
	void UnbindPixelShaderSRV(uint nSlot) { ID3D11ShaderResourceView* d3dBlankSRV = nullptr; m_d3dContext->PSSetShaderResources(nSlot, 1, &d3dBlankSRV); }
	void UnbindHullShaderSRV(uint nSlot) { ID3D11ShaderResourceView* d3dBlankSRV = nullptr; m_d3dContext->HSSetShaderResources(nSlot, 1, &d3dBlankSRV); }
	void UnbindDomainShaderSRV(uint nSlot) { ID3D11ShaderResourceView* d3dBlankSRV = nullptr; m_d3dContext->DSSetShaderResources(nSlot, 1, &d3dBlankSRV); }
	void UnbindGeometryShaderSRV(uint nSlot) { ID3D11ShaderResourceView* d3dBlankSRV = nullptr; m_d3dContext->GSSetShaderResources(nSlot, 1, &d3dBlankSRV); }
	void UnbindComputeShaderSRV(uint nSlot) { ID3D11ShaderResourceView* d3dBlankSRV = nullptr; m_d3dContext->CSSetShaderResources(nSlot, 1, &d3dBlankSRV); }

	void InitializeScreenQuad();
	void UpdateShaderMacros(); // Update any shaders that have macro variables
	void RecreateIBuffer();
	void RecreateVBuffer();
	void Map(ID3D11Buffer* d3dBuffer, D3D11_MAP d3dMapType, void* vSource, uint nSize, uint nOffset = 0);

public:
	Renderer();
	~Renderer();

	void Initialize();
	void ReInitialize();

	void Update();

	void RenderLine(float3 fStart, float3 fEnd, float3 fColor = { 1.0f, 1.0f, 1.0f }); // Debug lines

	void AddRenderObject(RenderObject* cObject, bool bMeshOnly = false);
	void RemoveRenderObject(RenderObject* cObject);

	void AddLight(PointLight* cLight) { m_cLightArr.push_back(cLight); }
	void RemoveLight(PointLight* cLight) { VECTOR_REMOVE(m_cLightArr, cLight); }

	void SetSkybox(RenderObject* cSkybox) { AddRenderObject(cSkybox, true); m_cSkybox = cSkybox; }
	void SetDirectionalLight(DirectionalLight* cDLight) { m_cDirectionalLight = cDLight; }

	void SetFullScreen(bool bFullScreen);
	void SetResolution(uint nWidth, uint nHeight);

	void ClearScene();
	void ToggleDebugCamera() { m_bDebugCamera = !m_bDebugCamera; }
	
	// Accessors // - Pointer or reference return types also act as mutators
	uint GetSampleCount() { return m_nSampleCount; }
	uint GetSampleQuality() { return m_nSampleQuality; }

	float GetNearPlane() { return m_fNearPlane; }
	float GetFarPlane() { return m_fFarPlane; }
	float4x4& GetViewMatrix() { return m_fViewMatrix; }
	float4x4& GetProjectionMatrix() { return m_fProjectionMatrix; }

	RenderObject* GetSkybox() { return m_cSkybox; }
	vector<RenderNode>& GetRenderArr() { return m_cRenderArr; }
	vector<RenderNode>& GetTransparentArr() { return m_cTransparentArr; }

	DirectionalLight* GetDirectionalLight() { return m_cDirectionalLight; }
	vector<PointLight*> GetLightArr() { return m_cLightArr; }
	
	unordered_map<wstring, ID3D11ShaderResourceView*>& GetTextureList() { return m_d3dTextureList; }

	ID3D11Device* GetDevice() { return m_d3dDevice; }
	ID3D11DeviceContext* GetContext() { return m_d3dContext; }
	IDXGISwapChain* GetSwapChain() { return m_d3dSwapChain; }

	ID3D11SamplerState* GetSamplerState() { return m_d3dSamplerState; }

	ID3D11RenderTargetView* GetCurrentRTV() { return m_d3dCurrentRTV; }
	ID3D11DepthStencilView* GetCurrentDSV() { return m_d3dCurrentDSV; }

	ID3D11Texture2D* GetBackBuffer() { return m_d3dBackBuffer; }
	ID3D11RenderTargetView* GetBackBufferRTV() { return m_d3dBackBufferRTV; }

	ID3D11Texture2D* GetSceneTexture() { return m_d3dSceneTexture; }
	ID3D11RenderTargetView* GetSceneTextureRTV() { return m_d3dSceneTextureRTV; }
	ID3D11ShaderResourceView* GetSceneTextureSRV() { return m_d3dSceneTextureSRV; }

	ID3D11Texture2D* GetDepthBuffer() { return m_d3dDepthBuffer; }
	ID3D11DepthStencilView* GetDepthBufferDSV() { return m_d3dDepthBufferDSV; }
	ID3D11ShaderResourceView* GetDepthBufferSRV() { return m_d3dDepthBufferSRV; }

	ID3D11BlendState* GetTransparentState() { return m_d3dTransparentState; }
	ID3D11RasterizerState* GetSolidState() { return m_d3dSolidState; }
	ID3D11RasterizerState* GetWireframeState() { return m_d3dWireframeState; }

	ID3D11Buffer* GetIndexBuffer() { return m_d3dIndexBuffer; }
	ID3D11Buffer* GetVertexBuffer() { return m_d3dVertexBuffer; }

	ID3D11Buffer* GetTransformBuffer() { return m_d3dTransformBuffer; }
	ID3D11Buffer* GetViewBuffer() { return m_d3dViewBuffer; }
	ID3D11Buffer* GetProjectionBuffer() { return m_d3dProjectionBuffer; }

	ID3D11Buffer* GetPropertyBuffer() { return m_d3dPropertyBuffer; }
	ID3D11Buffer* GetDLightBuffer() { return m_d3dDLightBuffer; }
	ID3D11Buffer* GetLightBuffer() { return m_d3dLightBuffer; }
	ID3D11Buffer* GetCameraBuffer() { return m_d3dCameraBuffer; }

	VertexShader* GetSkyboxVS() { return m_d3dSkyboxVS; }
	VertexShader* GetStandardVS() { return m_d3dStandardVS; }
	VertexShader* GetScreenVS() { return m_d3dScreenVS; }

	PixelShader* GetSkyboxPS() { return m_d3dSkyboxPS; }
	PixelShader* GetStandardPS() { return m_d3dStandardPS; }
	PixelShader* GetScreenPS() { return m_d3dScreenPS; }

	ID3D11InputLayout* GetVertex2DLayout() { return m_d3dVertex2DLayout; }
	ID3D11InputLayout* GetVertex3DLayout() { return m_d3dVertex3DLayout; }
};