#pragma once
#include "Renderer.h"

#define MAX_LIGHTS 512

struct DirectionalLight
{
	float m_fDirection[4];
	float m_fColor[4];
};

struct PointLight
{
	float m_fPosition[4];
	float m_fColor[4];
	float m_fRadius;
	float m_fBrightness;
	float m_fPadding[2];

	float DistanceToCamera(float4x4& fCamera)
	{
		XMVECTOR fStart = { m_fPosition[0], m_fPosition[1], m_fPosition[2] };
		XMVECTOR fEnd = { fCamera.m[3][0], fCamera.m[3][1], fCamera.m[3][2] };

		return XMVector3LengthSq(fEnd - fStart).m128_f32[0];
	}

	bool operator<(PointLight& cOther)
	{
		return DistanceToCamera(GRAPHICS->GetViewMatrix()) < cOther.DistanceToCamera(GRAPHICS->GetViewMatrix());
	}
};