#pragma once
#include "Lights.h"
#include "Renderer.h"
#include "RenderObject.h"

class Renderer;
class Game
{
	Renderer graphics;

	Mesh* cSkyboxMesh = nullptr;
	RenderObject* cSkybox = nullptr;

	Mesh* cFloorMesh = nullptr;
	RenderObject* cTop = nullptr;
	RenderObject* cLeft = nullptr;
	RenderObject* cRight = nullptr;
	RenderObject* cFront = nullptr;
	RenderObject* cBack = nullptr;
	RenderObject* cBottom = nullptr;

	uint nCubeDistance = 3;
	Mesh* cCubeMesh = nullptr;
	vector<RenderObject*> cCubes;

	uint nLength = 15;
	uint nWidth = 15;

	PointLight* tLight = nullptr;
	PointLight* tLight2 = nullptr;
	PointLight* tLight3 = nullptr;

	// Resolution Test //
	struct Resolution
	{
		uint nWidth;
		uint nHeight;
	};

	bool bFullScreen = false;
	float fPause = 0.15f;
	float fPauseTime = 0.0f;
	int nCurrentRes = 0;
	vector<Resolution> cResList;

	void CreateSkybox();
	void CreateRoom();
	void CreateCube();
	void CreateLights();

public:
	Game();
	~Game();

	void Update();
};