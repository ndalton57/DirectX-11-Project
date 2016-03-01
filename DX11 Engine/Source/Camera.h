#pragma once
#include "stdafx.h"
#include "MathWrapper.inl"

class Camera
{
	POINT m_nCurrentPos;
	POINT m_nPreviousPos;

	float m_fCameraAngle = 0.0f;
	float4x4 m_fViewMatrix;

public:
	Camera() { IdentityMatrix(m_fViewMatrix); }
	~Camera() = default;

	Camera(float4x4 fViewMatrix) { m_fViewMatrix = fViewMatrix; }

	void Update();
};