#include "Game.h"
#include "Shader.h"
#include "Vertex.h"
#include "Renderer.h"
#include "RenderObject.h"
#include "MathWrapper.inl"

Game::Game()
{
	CreateSkybox();

	CreateRoom();

	//CreateCube();

	CreateLights();

	cResList.push_back({ 1280, 720 });
	cResList.push_back({ 1600, 900 });
	cResList.push_back({ 1920, 1080 });
	cResList.push_back({ 2560, 1440 });
	cResList.push_back({ 3440, 1440 });

	GRAPHICS->SetResolution((uint)(gScreenWidth / 1.5f), (uint)(gScreenHeight / 1.5f)); // Initializes to 1280x720 if on 1080p screen
}

Game::~Game()
{
	SAFE_DELETE(cSkyboxMesh);
	SAFE_DELETE(cSkybox);

	SAFE_DELETE(cFloorMesh);
	SAFE_DELETE(cTop);
	SAFE_DELETE(cLeft);
	SAFE_DELETE(cRight);
	SAFE_DELETE(cFront);
	SAFE_DELETE(cBack);
	SAFE_DELETE(cBottom);

	SAFE_DELETE(cCubeMesh);
	VECTOR_DELETE_ALL(cCubes);

	SAFE_DELETE(tLight);
	SAFE_DELETE(tLight2);
}

void Game::Update()
{
	GRAPHICS->Update();

	tLight->m_fPosition[0] = sin(CURRENT_TIME * 0.6f) * 20.0f;
	tLight->m_fPosition[2] = cos(CURRENT_TIME * 0.6f) * 20.0f;

	tLight2->m_fPosition[0] = -sin(CURRENT_TIME) * 5.0f;
	tLight2->m_fPosition[2] = cos(CURRENT_TIME) * 5.0f;

	//RotateLocalZ(cFront->GetTransformMatrix(), DELTA_TIME * 10.0f);

	if (fPauseTime >= 0.0f)
		fPauseTime -= DELTA_TIME;

	if (GetAsyncKeyState(VK_F11) && fPauseTime <= 0.0f) // Don't have a keyboard manager, so this'll have to do
	{
		fPauseTime = fPause;
		bFullScreen = !bFullScreen;
		GRAPHICS->SetFullScreen(bFullScreen);

		if (bFullScreen)
			GRAPHICS->SetResolution(gScreenWidth, gScreenHeight);
		else
			GRAPHICS->SetResolution((uint)(gScreenWidth / 1.5f), (uint)(gScreenHeight / 1.5f));
	}
}

void Game::CreateSkybox()
{
	cSkyboxMesh = new Mesh();

	// SkyBox Indices Initialization //
	std::vector<uint> nSkyIndex;

	// Front Face //
	cSkyboxMesh->m_nIndexArr.push_back(1);
	cSkyboxMesh->m_nIndexArr.push_back(0);
	cSkyboxMesh->m_nIndexArr.push_back(3);

	cSkyboxMesh->m_nIndexArr.push_back(1);
	cSkyboxMesh->m_nIndexArr.push_back(3);
	cSkyboxMesh->m_nIndexArr.push_back(2);

	// Left Face //
	cSkyboxMesh->m_nIndexArr.push_back(0);
	cSkyboxMesh->m_nIndexArr.push_back(4);
	cSkyboxMesh->m_nIndexArr.push_back(7);

	cSkyboxMesh->m_nIndexArr.push_back(0);
	cSkyboxMesh->m_nIndexArr.push_back(7);
	cSkyboxMesh->m_nIndexArr.push_back(3);

	// Right Face //
	cSkyboxMesh->m_nIndexArr.push_back(5);
	cSkyboxMesh->m_nIndexArr.push_back(1);
	cSkyboxMesh->m_nIndexArr.push_back(2);

	cSkyboxMesh->m_nIndexArr.push_back(5);
	cSkyboxMesh->m_nIndexArr.push_back(2);
	cSkyboxMesh->m_nIndexArr.push_back(6);

	// Back Face //
	cSkyboxMesh->m_nIndexArr.push_back(4);
	cSkyboxMesh->m_nIndexArr.push_back(5);
	cSkyboxMesh->m_nIndexArr.push_back(6);

	cSkyboxMesh->m_nIndexArr.push_back(4);
	cSkyboxMesh->m_nIndexArr.push_back(6);
	cSkyboxMesh->m_nIndexArr.push_back(7);

	// Top Face //
	cSkyboxMesh->m_nIndexArr.push_back(0);
	cSkyboxMesh->m_nIndexArr.push_back(1);
	cSkyboxMesh->m_nIndexArr.push_back(5);

	cSkyboxMesh->m_nIndexArr.push_back(0);
	cSkyboxMesh->m_nIndexArr.push_back(5);
	cSkyboxMesh->m_nIndexArr.push_back(4);

	// Bottom Face //
	cSkyboxMesh->m_nIndexArr.push_back(7);
	cSkyboxMesh->m_nIndexArr.push_back(6);
	cSkyboxMesh->m_nIndexArr.push_back(2);

	cSkyboxMesh->m_nIndexArr.push_back(7);
	cSkyboxMesh->m_nIndexArr.push_back(2);
	cSkyboxMesh->m_nIndexArr.push_back(3);

	cSkyboxMesh->m_cVertexArr.push_back({ { -1, 1, -1, 1 } }); // 0 // Front Left  Top
	cSkyboxMesh->m_cVertexArr.push_back({ { 1, 1, -1, 1 } }); // 1 // Front Right Top
	cSkyboxMesh->m_cVertexArr.push_back({ { 1, -1, -1, 1 } }); // 2 // Front Right Bottom
	cSkyboxMesh->m_cVertexArr.push_back({ { -1, -1, -1, 1 } }); // 3 // Front Left  Bottom
	cSkyboxMesh->m_cVertexArr.push_back({ { -1, 1, 1, 1 } }); // 4 // Back  Left  Top
	cSkyboxMesh->m_cVertexArr.push_back({ { 1, 1, 1, 1 } }); // 5 // Back  Right Top
	cSkyboxMesh->m_cVertexArr.push_back({ { 1, -1, 1, 1 } }); // 6 // Back  Right Bottom
	cSkyboxMesh->m_cVertexArr.push_back({ { -1, -1, 1, 1 } }); // 7 // Back  Left  Bottom

	cSkybox = new RenderObject(cSkyboxMesh, { GRAPHICS->GetSkyboxVS(), GRAPHICS->GetSkyboxPS(), nullptr, nullptr, nullptr }, L"Assets/Skybox.dds");

	GRAPHICS->SetSkybox(cSkybox);
}

void Game::CreateRoom()
{
	cFloorMesh = new Mesh();

	cFloorMesh->m_nIndexArr.push_back(0);
	cFloorMesh->m_nIndexArr.push_back(1);
	cFloorMesh->m_nIndexArr.push_back(2);

	cFloorMesh->m_nIndexArr.push_back(0);
	cFloorMesh->m_nIndexArr.push_back(2);
	cFloorMesh->m_nIndexArr.push_back(3);

	cFloorMesh->m_cVertexArr.push_back
		({
			{ -25, 0, 25, 1 },
			{ 0, 0, 0, 10 },
			{ 0, 1, 0, 1 }
	});

	cFloorMesh->m_cVertexArr.push_back
		({
			{ 25, 0, 25, 1 },
			{ 10, 0, 0, 10 },
			{ 0, 1, 0, 1 }
	});

	cFloorMesh->m_cVertexArr.push_back
		({
			{ 25, 0, -25, 1 },
			{ 10, 10, 0, 10 },
			{ 0, 1, 0, 1 }
	});

	cFloorMesh->m_cVertexArr.push_back
		({
			{ -25, 0, -25, 1 },
			{ 0, 10, 0, 10 },
			{ 0, 1, 0, 1 }
	});

	cTop = new RenderObject(cFloorMesh, { GRAPHICS->GetStandardVS(), GRAPHICS->GetStandardPS() }, L"Assets/Hex.dds", L"Assets/HexNormals.dds");
	cLeft = new RenderObject(cFloorMesh, { GRAPHICS->GetStandardVS(), GRAPHICS->GetStandardPS() }, L"Assets/Hex.dds", L"Assets/HexNormals.dds");
	cRight = new RenderObject(cFloorMesh, { GRAPHICS->GetStandardVS(), GRAPHICS->GetStandardPS() }, L"Assets/Hex.dds", L"Assets/HexNormals.dds");
	cFront = new RenderObject(cFloorMesh, { GRAPHICS->GetStandardVS(), GRAPHICS->GetStandardPS() }, L"Assets/Hex.dds", L"Assets/HexNormals.dds");
	cBack = new RenderObject(cFloorMesh, { GRAPHICS->GetStandardVS(), GRAPHICS->GetStandardPS() }, L"Assets/Hex.dds", L"Assets/HexNormals.dds");
	cBottom = new RenderObject(cFloorMesh, { GRAPHICS->GetStandardVS(), GRAPHICS->GetStandardPS() }, L"Assets/Hex.dds", L"Assets/HexNormals.dds");

	TranslateLocalY(cTop->GetTransformMatrix(), 50.0f);

	TranslateLocalX(cLeft->GetTransformMatrix(), -25.0f);
	TranslateLocalY(cLeft->GetTransformMatrix(), 25.0f);

	TranslateLocalX(cRight->GetTransformMatrix(), 25.0f);
	TranslateLocalY(cRight->GetTransformMatrix(), 25.0f);

	TranslateLocalZ(cFront->GetTransformMatrix(), 25.0f);
	TranslateLocalY(cFront->GetTransformMatrix(), 25.0f);

	TranslateLocalZ(cBack->GetTransformMatrix(), -25.0f);
	TranslateLocalY(cBack->GetTransformMatrix(), 25.0f);

	RotateLocalZ(cTop->GetTransformMatrix(), 180.0f);
	RotateLocalZ(cLeft->GetTransformMatrix(), -90.0f);
	RotateLocalZ(cRight->GetTransformMatrix(), 90.0f);
	RotateLocalX(cFront->GetTransformMatrix(), -90.0f);
	RotateLocalX(cBack->GetTransformMatrix(), 90.0f);

	GRAPHICS->AddRenderObject(cTop);
	GRAPHICS->AddRenderObject(cLeft);
	GRAPHICS->AddRenderObject(cRight);
	GRAPHICS->AddRenderObject(cFront);
	GRAPHICS->AddRenderObject(cBack);
	GRAPHICS->AddRenderObject(cBottom);
}

void Game::CreateCube()
{
	cCubeMesh = new Mesh();

	for (uint i = 0; i < 6; i++) // Six sides
	{
		uint a = i * 4;
		cCubeMesh->m_nIndexArr.push_back(a);
		cCubeMesh->m_nIndexArr.push_back(a + 1);
		cCubeMesh->m_nIndexArr.push_back(a + 2);

		cCubeMesh->m_nIndexArr.push_back(a);
		cCubeMesh->m_nIndexArr.push_back(a + 2);
		cCubeMesh->m_nIndexArr.push_back(a + 3);
	}

	// Front Face //
	cCubeMesh->m_cVertexArr.push_back
	({
		{ -1, 1, -1, 1 },
		{ 0, 0, 0, 0 },
		{ 0, 0, -1, 0 }
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ 1, 1, -1, 1 },
		{ 1, 0, 0, 0 },
		{ 0, 0, -1, 0 }
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ 1, -1, -1, 1 },
		{ 1, 1, 0, 0 },
		{ 0, 0, -1, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ -1, -1, -1, 1 },
		{ 0, 1, 0, 0 },
		{ 0, 0, -1, 0 },
	});

	// Left Face //
	cCubeMesh->m_cVertexArr.push_back
	({
		{ -1, 1, 1, 1 },
		{ 0, 0, 0, 0 },
		{ -1, 0, 0, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ -1, 1, -1, 1 },
		{ 1, 0, 0, 0 },
		{ -1, 0, 0, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ -1, -1, -1, 1 },
		{ 1, 1, 0, 0 },
		{ -1, 0, 0, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ -1, -1, 1, 1 },
		{ 0, 1, 0, 0 },
		{ -1, 0, 0, 0 },
	});

	// Right Face //
	cCubeMesh->m_cVertexArr.push_back
	({
		{ 1, 1, -1, 1 },
		{ 0, 0, 0, 0 },
		{ 1, 0, 0, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ 1, 1, 1, 1 },
		{ 1, 0, 0, 0 },
		{ 1, 0, 0, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ 1, -1, 1, 1 },
		{ 1, 1, 0, 0 },
		{ 1, 0, 0, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ 1, -1, -1, 1 },
		{ 0, 1, 0, 0 },
		{ 1, 0, 0, 0 },
	});

	// Back Face //
	cCubeMesh->m_cVertexArr.push_back
	({
		{ 1, 1, 1, 1 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 1, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ -1, 1, 1, 1 },
		{ 1, 0, 0, 0 },
		{ 0, 0, 1, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ -1, -1, 1, 1 },
		{ 1, 1, 0, 0 },
		{ 0, 0, 1, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ 1, -1, 1, 1 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
	});

	// Top Face //
	cCubeMesh->m_cVertexArr.push_back
	({
		{ -1, 1, 1, 1 },
		{ 0, 0, 0, 0 },
		{ 0, 1, 0, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ 1, 1, 1, 1 },
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ 1, 1, -1, 1 },
		{ 1, 1, 0, 0 },
		{ 0, 1, 0, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ -1, 1, -1, 1 },
		{ 0, 1, 0, 0 },
		{ 0, 1, 0, 0 },
	});

	// Bottom Face //
	cCubeMesh->m_cVertexArr.push_back
	({
		{ -1, -1, -1, 1 },
		{ 0, 0, 0, 0 },
		{ 0, -1, 0, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ 1, -1, -1, 1 },
		{ 1, 0, 0, 0 },
		{ 0, -1, 0, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ 1, -1, 1, 1 },
		{ 1, 1, 0, 0 },
		{ 0, -1, 0, 0 },
	});

	cCubeMesh->m_cVertexArr.push_back
	({
		{ -1, -1, 1, 1 },
		{ 0, 1, 0, 0 },
		{ 0, -1, 0, 0 },
	});
}

void Game::CreateLights()
{
	// First Light //
	tLight = new PointLight
	({
		{ 50, 5, 0, 1 },
		{ 0, 1, 1 },
		{ 30 },
		{ 2.5f }
	});

	GRAPHICS->AddLight(tLight);

	// Second Light //
	tLight2 = new PointLight
	({
		{ 0, 5, 0, 1 },
		{ 1, 0.4f, 0 },
		{ 30 },
		{ 2.5f }
	});
	
	GRAPHICS->AddLight(tLight2);
}