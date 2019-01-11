#include "Renderer.h"

// Precompiled Shaders //
#include "Shaders/Includes/SkyboxVS.h"
#include "Shaders/Includes/StandardVS.h"
#include "Shaders/Includes/ScreenVS.h"

#include "Shaders/Includes/SkyboxPS.h"
#include "Shaders/Includes/StandardPS.h"
#include "Shaders/Includes/ScreenPS.h"

#include "Lights.h"
#include "Camera.h"
#include "Shader.h"
#include "RenderNode.h"
#include "RenderObject.h"
#include "DX11Helpers.inl"
#include "MathWrapper.inl"

const void* gGraphics = nullptr;
bool once = false;

Renderer::Renderer()
{
	gGraphics = this;

	m_fClearColor[0] = 0;
	m_fClearColor[1] = 0;
	m_fClearColor[2] = 0;
	m_fClearColor[3] = 0;

	#if _DEBUG
		bool bDebugMode = true;
		m_nSampleCount = 1;
		m_nSampleQuality = 0;
	#else
		bool bDebugMode = false;
		m_nSampleCount = 1;
		m_nSampleQuality = 0;
	#endif

	HRESULT hResult;

	// Device & Swap Chain Creation //
	DXGI_SWAP_CHAIN_DESC d3dSwapChainDesc;
	CreateSwapChainDesc(WINDOW_WIDTH, WINDOW_HEIGHT, m_nSampleCount, m_nSampleQuality, d3dSwapChainDesc);

	D3D_FEATURE_LEVEL d3dFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	hResult = D3D11CreateDeviceAndSwapChain(nullptr, // Default adapter
											D3D_DRIVER_TYPE_HARDWARE, // Render with hardware
											nullptr, // No need for software rasterizer
											(bDebugMode) ? D3D11_CREATE_DEVICE_DEBUG : NULL, // Debug mode check
											d3dFeatureLevels, // Supported DirectX versions
											sizeof(d3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL), // Number of supported DirectX versions
											D3D11_SDK_VERSION, // No other options
											&d3dSwapChainDesc, // Swap chain options
											&m_d3dSwapChain, // Swap chain output param (out)
											&m_d3dDevice, // Device output param (out)
											NULL, // Current DirectX version (out)
											&m_d3dContext); // Device context output param (out)

	assert("Device creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dDevice, "Device");
	DX_NAME(m_d3dContext, "Device Context");
	DX_NAME(m_d3dSwapChain, "Swap Chain");


	// Viewport Creation //
	D3D11_VIEWPORT d3dViewport;
	CreateViewport(WINDOW_WIDTH, WINDOW_HEIGHT, d3dViewport);

	m_d3dContext->RSSetViewports(1, &d3dViewport);


	// Sampler State Creation //
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	CreateSamplerDesc(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, d3dSamplerDesc);

	hResult = m_d3dDevice->CreateSamplerState(&d3dSamplerDesc, &m_d3dSamplerState);
	assert("Sampler state creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dSamplerState, "Sampler State");

	m_d3dContext->PSSetSamplers(NULL, 1, &m_d3dSamplerState);


	// Back Buffer Creation //
	hResult = m_d3dSwapChain->GetBuffer(NULL, __uuidof(m_d3dBackBuffer), (void**)(&m_d3dBackBuffer)); // Grab texture
	assert("Back buffer retrieval failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dBackBuffer, "Back Buffer");

	hResult = m_d3dDevice->CreateRenderTargetView(m_d3dBackBuffer, nullptr, &m_d3dBackBufferRTV); // Create RTV
	assert("Back buffer RTV creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dBackBufferRTV, "Back Buffer RTV");


	// Scene Texture Creation //
	D3D11_TEXTURE2D_DESC d3dTextureDesc;
	CreateTextureDesc(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT, m_nSampleCount, m_nSampleQuality,
					  D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, NULL, NULL, d3dTextureDesc);

	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDSVDesc;
	CreateDSVDesc(NULL, DXGI_FORMAT_D32_FLOAT, D3D11_DSV_DIMENSION_TEXTURE2DMS, d3dDSVDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc;
	CreateSRVDesc(DXGI_FORMAT_R32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE2DMS, d3dSRVDesc);

	hResult = m_d3dDevice->CreateTexture2D(&d3dTextureDesc, NULL, &m_d3dSceneTexture); // Create texture
	assert("Scene texture creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dSceneTexture, "Scene Texture");

	hResult = m_d3dDevice->CreateRenderTargetView(m_d3dSceneTexture, nullptr, &m_d3dSceneTextureRTV); // Create RTV
	assert("Scene texture RTV creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dSceneTextureRTV, "Scene Texture RTV");

	hResult = m_d3dDevice->CreateShaderResourceView(m_d3dSceneTexture, nullptr, &m_d3dSceneTextureSRV); // Create SRV
	assert("Scene texture SRV creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dSceneTextureSRV, "Scene Texture SRV");


	// Depth Buffer Creation //
	CreateTextureDesc(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R32_TYPELESS, m_nSampleCount, m_nSampleQuality, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, 
					  NULL, NULL, d3dTextureDesc);

	CreateDSVDesc(NULL, DXGI_FORMAT_D32_FLOAT, D3D11_DSV_DIMENSION_TEXTURE2DMS, d3dDSVDesc);

	CreateSRVDesc(DXGI_FORMAT_R32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE2DMS, d3dSRVDesc);

	hResult = m_d3dDevice->CreateTexture2D(&d3dTextureDesc, NULL, &m_d3dDepthBuffer); // Create texture
	assert("Depth buffer creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dDepthBuffer, "Depth Buffer");

	hResult = m_d3dDevice->CreateDepthStencilView(m_d3dDepthBuffer, &d3dDSVDesc, &m_d3dDepthBufferDSV); // Create DSV
	assert("Depth buffer DSV creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dDepthBufferDSV, "Depth Buffer DSV");

	hResult = m_d3dDevice->CreateShaderResourceView(m_d3dDepthBuffer, &d3dSRVDesc, &m_d3dDepthBufferSRV); // Create SRV
	assert("Depth buffer SRV creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dDepthBufferSRV, "Depth Buffer SRV");


	// Transparent State Creation //
	D3D11_BLEND_DESC d3dBlendDesc;
	CreateBlendDesc(d3dBlendDesc);

	hResult = m_d3dDevice->CreateBlendState(&d3dBlendDesc, &m_d3dTransparentState);
	assert("Transparent blend state creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dTransparentState, "Transparent State");


	// Solid State Creation //
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	CreateRasterizerDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, true, d3dRasterizerDesc);

	hResult = m_d3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_d3dSolidState);
	assert("Solid rasterizer state creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dSolidState, "Solid State");


	// Wireframe State Creation //
	CreateRasterizerDesc(D3D11_FILL_WIREFRAME, D3D11_CULL_BACK, true, d3dRasterizerDesc);

	hResult = m_d3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_d3dWireframeState);
	assert("Wireframe rasterizer state creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dWireframeState, "Wireframe State");


	// Index Buffer Creation //
	D3D11_BUFFER_DESC d3dBufferDesc;
	CreateBufferDesc(sizeof(uint), D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER,
					 D3D11_CPU_ACCESS_WRITE, sizeof(uint), d3dBufferDesc);

	hResult = m_d3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_d3dIndexBuffer);
	assert("Index buffer creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dIndexBuffer, "Index Buffer");

	m_d3dContext->IASetIndexBuffer(m_d3dIndexBuffer, DXGI_FORMAT_R32_UINT, NULL);


	// Vertex Buffer Creation //
	CreateBufferDesc(sizeof(Vertex3D), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER,
					 D3D11_CPU_ACCESS_WRITE, sizeof(Vertex3D), d3dBufferDesc);

	hResult = m_d3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_d3dVertexBuffer);
	assert("Vertex buffer creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dVertexBuffer, "Vertex Buffer");

	uint nStride = sizeof(Vertex3D);
	uint nOffset = 0;
	m_d3dContext->IASetVertexBuffers(0, 1, &m_d3dVertexBuffer, &nStride, &nOffset);
	m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Transform Buffer Creation //
	CreateBufferDesc(sizeof(float4x4), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, NULL, d3dBufferDesc);

	hResult = m_d3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_d3dTransformBuffer);
	assert("Transform buffer creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dTransformBuffer, "Transform Buffer");

	m_d3dContext->VSSetConstantBuffers(0, 1, &m_d3dTransformBuffer); // b0


	// View Buffer Creation //
	CreateBufferDesc(sizeof(float4x4), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, NULL, d3dBufferDesc);

	hResult = m_d3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_d3dViewBuffer);
	assert("View buffer creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dViewBuffer, "View Buffer");

	m_d3dContext->VSSetConstantBuffers(1, 1, &m_d3dViewBuffer); // b1


	// Projection Buffer Creation //
	CreateBufferDesc(sizeof(float4x4), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, NULL, d3dBufferDesc);

	hResult = m_d3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_d3dProjectionBuffer);
	assert("Projection buffer creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dProjectionBuffer, "Projection Buffer");

	m_d3dContext->VSSetConstantBuffers(2, 1, &m_d3dProjectionBuffer); // b2


	// Property Buffer Creation //
	CreateBufferDesc(sizeof(float4), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, NULL, d3dBufferDesc);

	hResult = m_d3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_d3dPropertyBuffer);
	assert("Property buffer creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dPropertyBuffer, "Property Buffer");

	m_d3dContext->PSSetConstantBuffers(5, 1, &m_d3dPropertyBuffer); // b5


	// Directional Light Buffer Creation //
	CreateBufferDesc(sizeof(DirectionalLight), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, NULL, d3dBufferDesc);

	hResult = m_d3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_d3dDLightBuffer);
	assert("Directional light buffer creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dDLightBuffer, "Directional Light Buffer");

	m_d3dContext->PSSetConstantBuffers(6, 1, &m_d3dDLightBuffer); // b6


	// Light Buffer Creation //
	CreateBufferDesc(MAX_LIGHTS * sizeof(PointLight), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, NULL, d3dBufferDesc);

	hResult = m_d3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_d3dLightBuffer);
	assert("Light buffer creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dLightBuffer, "Light Buffer");

	m_d3dContext->PSSetConstantBuffers(7, 1, &m_d3dLightBuffer); // b7


	// Camera Buffer Creation //
	CreateBufferDesc(sizeof(float) * 4, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, NULL, d3dBufferDesc);

	hResult = m_d3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_d3dCameraBuffer);
	assert("Camera buffer creation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dCameraBuffer, "Camera Buffer");

	m_d3dContext->PSSetConstantBuffers(8, 1, &m_d3dCameraBuffer); // b8


	// Vertex2D Layout Creation //
	D3D11_INPUT_ELEMENT_DESC d3dElementDesc2D[2];
	CreateInputElementDesc("POSITIONS", DXGI_FORMAT_R32G32B32A32_FLOAT, d3dElementDesc2D[0]);
	CreateInputElementDesc("TEXCOORDS", DXGI_FORMAT_R32G32B32A32_FLOAT, d3dElementDesc2D[1]);


	// Vertex3D Layout Creation //
	D3D11_INPUT_ELEMENT_DESC d3dElementDesc3D[4];
	CreateInputElementDesc("POSITIONS", DXGI_FORMAT_R32G32B32A32_FLOAT, d3dElementDesc3D[0]);
	CreateInputElementDesc("TEXCOORDS", DXGI_FORMAT_R32G32B32A32_FLOAT, d3dElementDesc3D[1]);
	CreateInputElementDesc("NORMALS", DXGI_FORMAT_R32G32B32A32_FLOAT, d3dElementDesc3D[2]);
	CreateInputElementDesc("TANGENTS", DXGI_FORMAT_R32G32B32A32_FLOAT, d3dElementDesc3D[3]);


	// Vertex Shader Creations //
	LoadSkyboxVS(m_d3dSkyboxVS, d3dElementDesc3D, 4);
	LoadStandardVS(m_d3dStandardVS, d3dElementDesc3D, 4);
	LoadScreenVS(m_d3dScreenVS, d3dElementDesc2D, 2);


	// Pixel Shader Creations //
	LoadSkyboxPS(m_d3dSkyboxPS);
	LoadStandardPS(m_d3dStandardPS);
	LoadScreenPS(m_d3dScreenPS);


	// Matrix Creations //
	IdentityMatrix(m_fViewMatrix);
	TranslateLocalY(m_fViewMatrix, 10.0f);
	CreateProjectionMatrix(m_fNearPlane, m_fFarPlane, m_fProjectionMatrix);


	// Create Camera //
	m_cDebugCamera = new Camera(m_fViewMatrix);


	// Post Process Quad Initialization //
	//InitializeScreenQuad();
}

Renderer::~Renderer()
{
	ClearScene();

	SAFE_DELETE(m_cDebugCamera);

	SAFE_DELETE(m_cScreenQuadMesh);
	SAFE_DELETE(m_cScreenQuad);

	SAFE_DELETE(m_cDirectionalLight);
	VECTOR_DELETE_ALL(m_cLightArr);

	MAP_RELEASE_ALL(m_d3dTextureList);

	SAFE_RELEASE(m_d3dDevice);
	SAFE_RELEASE(m_d3dContext);
	SAFE_RELEASE(m_d3dSwapChain);
	
	SAFE_RELEASE(m_d3dSamplerState);

	SAFE_RELEASE(m_d3dBackBuffer);
	SAFE_RELEASE(m_d3dBackBufferRTV);

	SAFE_RELEASE(m_d3dSceneTexture);
	SAFE_RELEASE(m_d3dSceneTextureRTV);
	SAFE_RELEASE(m_d3dSceneTextureSRV);

	SAFE_RELEASE(m_d3dDepthBuffer);
	SAFE_RELEASE(m_d3dDepthBufferDSV);
	SAFE_RELEASE(m_d3dDepthBufferSRV);

	SAFE_RELEASE(m_d3dTransparentState);
	SAFE_RELEASE(m_d3dSolidState);
	SAFE_RELEASE(m_d3dWireframeState);

	SAFE_RELEASE(m_d3dIndexBuffer);
	SAFE_RELEASE(m_d3dVertexBuffer);

	SAFE_RELEASE(m_d3dTransformBuffer);
	SAFE_RELEASE(m_d3dViewBuffer);
	SAFE_RELEASE(m_d3dProjectionBuffer);

	SAFE_RELEASE(m_d3dPropertyBuffer);
	SAFE_RELEASE(m_d3dDLightBuffer);
	SAFE_RELEASE(m_d3dLightBuffer);
	SAFE_RELEASE(m_d3dCameraBuffer);

	SAFE_DELETE(m_d3dSkyboxVS);
	SAFE_DELETE(m_d3dStandardVS);
	SAFE_DELETE(m_d3dScreenVS);

	SAFE_DELETE(m_d3dSkyboxPS);
	SAFE_DELETE(m_d3dStandardPS);
	SAFE_DELETE(m_d3dScreenPS);

	SAFE_RELEASE(m_d3dVertex2DLayout);
	SAFE_RELEASE(m_d3dVertex3DLayout);
}

void Renderer::Update()
{
	m_cDebugCamera->Update();

	UpdateShaderMacros();

	MapBuffers();

	RenderPass();

	//PostProcess();

	Present();
}

void Renderer::RenderLine(float3 fStart, float3 fEnd, float3 fColor)
{
	// Create 2 vertex's based on input params and map to the VertexBuffer
}

void Renderer::AddRenderObject(RenderObject* cObject, bool bMeshOnly)
{
	// Mesh Handling //
	Mesh* cNewMesh = cObject->GetMesh();

	cNewMesh->m_nInstanceCount++; // An instance of cObject's mesh has been added
	if (cNewMesh->m_nInstanceCount == 1) // If this is the first instance of cObject's mesh being added
	{
		cNewMesh->m_nIndexStart = m_nIndexCount; // Set mesh's starting index
		cNewMesh->m_nVertexStart = m_nVertexCount; // Set mesh's starting vertex

		m_nIndexCount += (uint)cNewMesh->m_nIndexArr.size(); // Update total index count
		m_nVertexCount += (uint)cNewMesh->m_cVertexArr.size(); // Update total vertex count

		m_cMeshes.push_back(cNewMesh); // Add to exisiting mesh list
		
		// Toggle GPU Update //
		m_bMeshUpdate = true;
	}

	if (bMeshOnly) // For things that manually render, like the skybox and screen quad
		return;

	// Object Handling //
	if (cObject->GetTransparent() == false) // Regular objects
	{
		uint nIndex = 0;

		if (VECTOR_FIND_AND_INDEX(m_cRenderArr, cObject, nIndex)) // Check if a compatible node exists and grab it's index
			m_cRenderArr[nIndex].AddObject(cObject);
		else // No existing node is compatible with the new object
			m_cRenderArr.push_back(RenderNode(cObject)); // Create new node using cObject's context
	}
	else // Transparent Objects
	{
		uint nIndex = 0;

		if (VECTOR_FIND_AND_INDEX(m_cTransparentArr, cObject, nIndex)) // Check if a compatible node exists and grab it's index
			m_cTransparentArr[nIndex].AddObject(cObject);
		else // No existing node is compatible with the new object
			m_cTransparentArr.push_back(RenderNode(cObject)); // Create new node using cObject's context
	}
}

void Renderer::RemoveRenderObject(RenderObject* cObject)
{
	if (cObject->GetMesh()->m_nInstanceCount == 0) // If there aren't any instances of this object to begin with
		return;

	// Mesh Handling //
	Mesh* cRemoveMesh = cObject->GetMesh(); // Grab cObject's mesh

	cRemoveMesh->m_nInstanceCount--; // An instance of cObject's mesh has been removed
	if (cRemoveMesh->m_nInstanceCount == 0) // If no more instances of cObject's mesh are being rendered
	{
		m_nIndexCount -= (uint)cRemoveMesh->m_nIndexArr.size(); // Update total index count
		m_nVertexCount -= (uint)cRemoveMesh->m_cVertexArr.size(); // Update total vertex count

		// Mesh Array Update //
		uint nIndexOffset = (uint)cRemoveMesh->m_nIndexArr.size();
		uint nVertexOffset = (uint)cRemoveMesh->m_cVertexArr.size();
		uint nMeshIndex = (uint)(VECTOR_INDEX(m_cMeshes, cRemoveMesh)); // Grab mesh's index in the array
		for (uint i = nMeshIndex + 1; i < m_cMeshes.size(); i++) // Start from the next mesh and loop to the end, updating each mesh's data along the way
		{
			m_cMeshes[i]->m_nIndexStart -= nIndexOffset;
			m_cMeshes[i]->m_nVertexStart -= nVertexOffset;
		}

		VECTOR_REMOVE(m_cMeshes, cRemoveMesh); // Remove from mesh list

		// Toggle GPU Update //
		m_bMeshUpdate = true;
	}

	// Object Handling //
	if (cObject->GetTransparent() == false) // Regular objects
	{
		uint nIndex = 0;

		if (VECTOR_FIND_AND_INDEX(m_cRenderArr, cObject, nIndex)) // Find node that would contain the object being removed
			m_cRenderArr[nIndex].Remove(cObject); // And remove it
	}
	else // Transparent objects
	{
		uint nIndex = 0;

		if (VECTOR_FIND_AND_INDEX(m_cTransparentArr, cObject, nIndex)) // Find node that would contain the object being removed
			m_cTransparentArr[nIndex].Remove(cObject); // And remove it
	}
}

void Renderer::SetFullScreen(bool bFullScreen)
{
	if (bFullScreen) // Change to fullscreen
		SetWindowPos(gWindow, NULL, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SWP_NOOWNERZORDER | SWP_NOZORDER);
	else // Change to windowed
		SetWindowPos(gWindow, NULL, WINDOW_START_X, WINDOW_START_Y, WINDOW_WIDTH, WINDOW_HEIGHT, SWP_NOOWNERZORDER | SWP_NOZORDER);

	m_bFullScreen = bFullScreen;
}

void Renderer::SetResolution(uint nWidth, uint nHeight)
{
	WINDOW_WIDTH = nWidth;
	WINDOW_HEIGHT = nHeight;

	// Resize And Reposition Window //
	if (m_bFullScreen) // Change to fullscreen
		SetWindowPos(gWindow, NULL, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SWP_NOOWNERZORDER | SWP_NOZORDER);
	else // Change to windowed
		SetWindowPos(gWindow, NULL, WINDOW_START_X, WINDOW_START_Y, WINDOW_WIDTH, WINDOW_HEIGHT, SWP_NOOWNERZORDER | SWP_NOZORDER);

	// Release Resolution-Dependant Variables //
	SAFE_RELEASE(m_d3dBackBuffer);
	SAFE_RELEASE(m_d3dBackBufferRTV);

	SAFE_RELEASE(m_d3dSceneTexture);
	SAFE_RELEASE(m_d3dSceneTextureRTV);
	SAFE_RELEASE(m_d3dSceneTextureSRV);

	SAFE_RELEASE(m_d3dDepthBuffer);
	SAFE_RELEASE(m_d3dDepthBufferDSV);
	SAFE_RELEASE(m_d3dDepthBufferSRV);

	// Clear RTV & DSV //
	m_d3dCurrentRTV = nullptr;
	m_d3dCurrentDSV = nullptr;
	m_d3dContext->OMSetRenderTargets(NULL, nullptr, nullptr);

	HRESULT hResult;

	// Resize Swap Chain //
	hResult = m_d3dSwapChain->ResizeBuffers(1, WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, NULL);
	assert("Swap chain resize failed!" && SUCCEEDED(hResult));


	// Recreate Viewport //
	D3D11_VIEWPORT d3dViewport;
	CreateViewport(WINDOW_WIDTH, WINDOW_HEIGHT, d3dViewport);

	m_d3dContext->RSSetViewports(1, &d3dViewport);


	// Recreate Back Buffer //
	hResult = m_d3dSwapChain->GetBuffer(NULL, __uuidof(m_d3dBackBuffer), (void**)(&m_d3dBackBuffer)); // Grab texture
	assert("Back buffer retrieval failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dBackBuffer, "Back Buffer");

	hResult = m_d3dDevice->CreateRenderTargetView(m_d3dBackBuffer, nullptr, &m_d3dBackBufferRTV); // Create RTV
	assert("Back buffer RTV resize failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dBackBufferRTV, "Back Buffer RTV");


	// Recreate Scene Texture //
	D3D11_TEXTURE2D_DESC d3dTextureDesc;
	CreateTextureDesc(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT, m_nSampleCount, m_nSampleQuality,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, NULL, NULL, d3dTextureDesc);

	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDSVDesc;
	CreateDSVDesc(NULL, DXGI_FORMAT_D32_FLOAT, D3D11_DSV_DIMENSION_TEXTURE2DMS, d3dDSVDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc;
	CreateSRVDesc(DXGI_FORMAT_R32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE2DMS, d3dSRVDesc);

	hResult = m_d3dDevice->CreateTexture2D(&d3dTextureDesc, NULL, &m_d3dSceneTexture); // Create texture
	assert("Scene texture resize failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dSceneTexture, "Scene Texture");

	hResult = m_d3dDevice->CreateRenderTargetView(m_d3dSceneTexture, nullptr, &m_d3dSceneTextureRTV); // Create RTV
	assert("Scene texture RTV resize failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dSceneTextureRTV, "Scene Texture RTV");

	hResult = m_d3dDevice->CreateShaderResourceView(m_d3dSceneTexture, nullptr, &m_d3dSceneTextureSRV); // Create SRV
	assert("Scene texture SRV resize failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dSceneTextureSRV, "Scene Texture SRV");


	// Recreate Depth Buffer //
	CreateTextureDesc(WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R32_TYPELESS, m_nSampleCount, m_nSampleQuality,
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, NULL, NULL, d3dTextureDesc);

	CreateDSVDesc(NULL, DXGI_FORMAT_D32_FLOAT, D3D11_DSV_DIMENSION_TEXTURE2DMS, d3dDSVDesc);

	CreateSRVDesc(DXGI_FORMAT_R32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE2DMS, d3dSRVDesc);

	hResult = m_d3dDevice->CreateTexture2D(&d3dTextureDesc, NULL, &m_d3dDepthBuffer); // Create texture
	assert("Depth buffer resize failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dDepthBuffer, "Depth Buffer");

	hResult = m_d3dDevice->CreateDepthStencilView(m_d3dDepthBuffer, &d3dDSVDesc, &m_d3dDepthBufferDSV); // Create DSV
	assert("Depth buffer DSV resize failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dDepthBufferDSV, "Depth Buffer DSV");

	hResult = m_d3dDevice->CreateShaderResourceView(m_d3dDepthBuffer, &d3dSRVDesc, &m_d3dDepthBufferSRV); // Create SRV
	assert("Depth buffer SRV resize failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dDepthBufferSRV, "Depth Buffer SRV");

	// Recreate Projection Matrices //
	CreateProjectionMatrix(m_fNearPlane, m_fFarPlane, m_fProjectionMatrix);
}

void Renderer::ClearScene()
{
	ClearDepth();
	ClearScreen();

	m_cSkybox = nullptr;
	m_cRenderArr.clear();
	m_cTransparentArr.clear();

	m_cDirectionalLight = nullptr;
	m_cLightArr.clear();
}

// ------------------------- //
// --- Private Functions --- //
// ------------------------- //

void Renderer::MapBuffers()
{
	InverseMatrix(m_fViewMatrix);
	Map(m_d3dViewBuffer, D3D11_MAP_WRITE_DISCARD, &m_fViewMatrix, sizeof(m_fViewMatrix)); // View matrix
	InverseMatrix(m_fViewMatrix);

	Map(m_d3dProjectionBuffer, D3D11_MAP_WRITE_DISCARD, &m_fProjectionMatrix, sizeof(m_fProjectionMatrix)); // Projection matrix

	if (m_cDirectionalLight) // If a directional light has been set
		Map(m_d3dDLightBuffer, D3D11_MAP_WRITE_DISCARD, &*m_cDirectionalLight, sizeof(*m_cDirectionalLight)); // Directional light

	// Send Lights To GPU // - Manual mapping because light vector holds onto pointers of the lights, and not the lights themselves
	D3D11_MAPPED_SUBRESOURCE d3dResource;
	ZeroMemory(&d3dResource, sizeof(d3dResource));
	m_d3dContext->Map(m_d3dLightBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &d3dResource);

	//sort(m_cLightArr.begin(), m_cLightArr.end()); // Sort lights by distance from camera
	for (uint i = 0; i < m_cLightArr.size() && i < MAX_LIGHTS; i++)
		memcpy((PointLight*)d3dResource.pData + i, &*(m_cLightArr[i]), sizeof(*m_cLightArr[i]));

	m_d3dContext->Unmap(m_d3dLightBuffer, NULL);

	Map(m_d3dCameraBuffer, D3D11_MAP_WRITE_DISCARD, m_fViewMatrix.m[3], sizeof(float) * 4); // Camera Position

	// Send New Mesh List To GPU //
	if (m_bMeshUpdate)
	{
		RecreateIBuffer();
		RecreateVBuffer();
		m_bMeshUpdate = false;
	}
}

void Renderer::RenderPass()
{
	SetRenderTarget(m_d3dBackBufferRTV);
	SetDepthStencil(m_d3dDepthBufferDSV);

	ClearDepth();
	ClearScreen();

	// Update Skybox //
	if (m_cSkybox)
	{
		SnapPosition(m_cSkybox->GetTransformMatrix(), m_fViewMatrix);
		m_cSkybox->Render(true);
		ClearDepth();
	}

	for (uint i = 0; i < m_cRenderArr.size(); i++) // Normal objects
		m_cRenderArr[i].Render();

	for (uint i = 0; i < m_cTransparentArr.size(); i++) // Transparent objects
		m_cTransparentArr[i].Render();
}

void Renderer::PostProcess()
{
	// Do Post Process Effects //

	// Render Final Image //
	SetRenderTarget(m_d3dBackBufferRTV);
	ClearScreen();

	m_d3dContext->PSSetShaderResources(3, 1, &m_d3dSceneTextureSRV);

	m_cScreenQuad->Render(true);
}

void Renderer::InitializeScreenQuad()
{
	m_cScreenQuadMesh = new Mesh();

	m_cScreenQuadMesh->m_nIndexArr.push_back(0);
	m_cScreenQuadMesh->m_nIndexArr.push_back(1);
	m_cScreenQuadMesh->m_nIndexArr.push_back(2);

	m_cScreenQuadMesh->m_nIndexArr.push_back(0);
	m_cScreenQuadMesh->m_nIndexArr.push_back(2);
	m_cScreenQuadMesh->m_nIndexArr.push_back(3);

	m_cScreenQuadMesh->m_cVertexArr.push_back
	({
		{ -1, 1, 0, 0 },
		{ 0, 0 }
	});

	m_cScreenQuadMesh->m_cVertexArr.push_back
	({
		{ 1, 1, 0, 0 },
		{ 1, 0 }
	});

	m_cScreenQuadMesh->m_cVertexArr.push_back
	({
		{ 1, -1, 0, 0 },
		{ 1, 1 }
	});

	m_cScreenQuadMesh->m_cVertexArr.push_back
	({
		{ -1, -1, 0, 0 },
		{ 0, 1 }
	});

	m_cScreenQuad = new RenderObject(m_cScreenQuadMesh, { m_d3dScreenVS, m_d3dScreenPS, nullptr, nullptr, nullptr });

	AddRenderObject(m_cScreenQuad);
}

void Renderer::UpdateShaderMacros()
{
	GRAPHICS->GetStandardPS()->Update({ (uint)m_cLightArr.size(), false });
}

void Renderer::RecreateIBuffer()
{
	SAFE_RELEASE(m_d3dIndexBuffer);

	D3D11_BUFFER_DESC d3dBufferDesc;
	CreateBufferDesc(m_nIndexCount * sizeof(uint), D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER, NULL,
					 sizeof(uint), d3dBufferDesc);

	vector<uint> nIndexCopies(m_nIndexCount);
	for (uint i = 0; i < m_cMeshes.size(); i++)
		memcpy(&nIndexCopies[0] + m_cMeshes[i]->m_nIndexStart, &m_cMeshes[i]->m_nIndexArr[0], m_cMeshes[i]->m_nIndexArr.size() * sizeof(uint));

	D3D11_SUBRESOURCE_DATA d3dBufferResource;
	CreateBufferResource(&nIndexCopies[0], d3dBufferResource);

	HRESULT hResult;
	hResult = m_d3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferResource, &m_d3dIndexBuffer);
	assert("Index buffer recreation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dIndexBuffer, "Index Buffer");

	m_d3dContext->IASetIndexBuffer(m_d3dIndexBuffer, DXGI_FORMAT_R32_UINT, NULL);
}

void Renderer::RecreateVBuffer()
{
	SAFE_RELEASE(m_d3dVertexBuffer);

	D3D11_BUFFER_DESC d3dBufferDesc;
	CreateBufferDesc(m_nVertexCount * sizeof(Vertex3D), D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, NULL,
					 sizeof(Vertex3D), d3dBufferDesc);

	vector<Vertex3D> cVertexCopies(m_nVertexCount);
	for (uint i = 0; i < m_cMeshes.size(); i++)
		memcpy(&cVertexCopies[0] + m_cMeshes[i]->m_nVertexStart, &m_cMeshes[i]->m_cVertexArr[0], m_cMeshes[i]->m_cVertexArr.size() * sizeof(Vertex3D));

	D3D11_SUBRESOURCE_DATA d3dBufferResource;
	CreateBufferResource(&cVertexCopies[0], d3dBufferResource);

	HRESULT hResult;
	hResult = m_d3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferResource, &m_d3dVertexBuffer);
	assert("Vertex buffer recreation failed!" && SUCCEEDED(hResult));
	DX_NAME(m_d3dVertexBuffer, "Vertex Buffer");

	uint nStride = sizeof(Vertex3D);
	uint nOffset = 0;
	m_d3dContext->IASetVertexBuffers(0, 1, &m_d3dVertexBuffer, &nStride, &nOffset);
}

void Renderer::Map(ID3D11Buffer* d3dBuffer, D3D11_MAP d3dMapType, void* vSource, uint nSize, uint nOffset)
{
	D3D11_MAPPED_SUBRESOURCE d3dResource;

	ZeroMemory(&d3dResource, sizeof(d3dResource));
	m_d3dContext->Map(d3dBuffer, NULL, d3dMapType, NULL, &d3dResource);
	memcpy((char*)d3dResource.pData + nOffset, vSource, nSize);
	m_d3dContext->Unmap(d3dBuffer, NULL);
}