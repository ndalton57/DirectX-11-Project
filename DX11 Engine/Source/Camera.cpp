#include "Camera.h"

#include "Renderer.h"
#include "MathWrapper.inl"

void Camera::Update()
{
	if (GetCursorPos(&m_nCurrentPos) && GetAsyncKeyState(0x02))
	{
		float fDeltaX = (float)(m_nCurrentPos.x - m_nPreviousPos.x);
		float fDeltaY = (float)(m_nCurrentPos.y - m_nPreviousPos.y);

		// Cursor Wrap //
		if (m_nCurrentPos.x < 1) // Left
			SetCursorPos(SCREEN_WIDTH - 2, m_nCurrentPos.y);
		if (m_nCurrentPos.y < 1) // Top
			SetCursorPos(m_nCurrentPos.x, SCREEN_HEIGHT - 2);
		if (m_nCurrentPos.x > SCREEN_WIDTH - 2) // Right
			SetCursorPos(1, m_nCurrentPos.y);
		if (m_nCurrentPos.y > SCREEN_HEIGHT - 2) // Bottom
			SetCursorPos(m_nCurrentPos.x, 1);

		// - Rotate Camera - //
		if (fDeltaX || fDeltaY)
		{
			fDeltaX *= SENSITIVITY;
			fDeltaY *= SENSITIVITY;

			// Store Position //
			float fMatrixPosX = m_fViewMatrix.m[3][0];
			float fMatrixPosY = m_fViewMatrix.m[3][1];
			float fMatrixPosZ = m_fViewMatrix.m[3][2];
			float fMatrixPosW = m_fViewMatrix.m[3][3];

			// Set Position To World Center // Global rotation becomes same as local
			m_fViewMatrix.m[3][0] = 0;
			m_fViewMatrix.m[3][1] = 0;
			m_fViewMatrix.m[3][2] = 0;
			m_fViewMatrix.m[3][3] = 0;

			// Rotate Globally On Y-Axis //
			RotateGlobalY(m_fViewMatrix, fDeltaX);

			if (m_fCameraAngle + fDeltaY <  90 &&
				m_fCameraAngle + fDeltaY > -90) // No x-axis 360's
			{
				// Update Angle //
				m_fCameraAngle += fDeltaY;

				// Rotate Locally On X-Axis //
				RotateLocalX(m_fViewMatrix, fDeltaY);
			}

			// Restore Position //
			m_fViewMatrix.m[3][0] = fMatrixPosX;
			m_fViewMatrix.m[3][1] = fMatrixPosY;
			m_fViewMatrix.m[3][2] = fMatrixPosZ;
			m_fViewMatrix.m[3][3] = fMatrixPosW;
		}
	}

	// Snapping Fix //
	if (m_nCurrentPos.x < 1 ||
		m_nCurrentPos.y < 1 ||
		m_nCurrentPos.x > SCREEN_WIDTH - 2 ||
		m_nCurrentPos.y > SCREEN_HEIGHT - 2)
	{
		GetCursorPos(&m_nCurrentPos);
	}

	m_nPreviousPos = m_nCurrentPos;

	// Distace Per Second //
	float fBoostSpeed = 90 * DELTA_TIME;
	float fNormalSpeed = 30 * DELTA_TIME;

	// - Key Movement - //
	if (GetAsyncKeyState(0x57)) // W
	{
		if (GetAsyncKeyState(0x10))
			TranslateLocalZ(m_fViewMatrix, fBoostSpeed); // Boost
		else
			TranslateLocalZ(m_fViewMatrix, fNormalSpeed); // Normal
	}
	if (GetAsyncKeyState(0x53)) // S
	{
		if (GetAsyncKeyState(0x10))
			TranslateLocalZ(m_fViewMatrix, -fBoostSpeed); // Boost
		else
			TranslateLocalZ(m_fViewMatrix, -fNormalSpeed); // Normal
	}
	if (GetAsyncKeyState(0x41)) // A
	{
		if (GetAsyncKeyState(0x10))
			TranslateLocalX(m_fViewMatrix, -fBoostSpeed); // Boost
		else
			TranslateLocalX(m_fViewMatrix, -fNormalSpeed); // Normal
	}
	if (GetAsyncKeyState(0x44)) // D
	{
		if (GetAsyncKeyState(0x10))
			TranslateLocalX(m_fViewMatrix, fBoostSpeed); // Boost
		else
			TranslateLocalX(m_fViewMatrix, fNormalSpeed); // Normal
	}

	GRAPHICS->GetViewMatrix() = m_fViewMatrix;
}