#pragma once
#include "stdafx.h"

inline XMFLOAT4X4 Convert(XMMATRIX& a)
{
	XMFLOAT4X4 d3dNewMesh;
	XMStoreFloat4x4(&d3dNewMesh, a);
	return d3dNewMesh;
}

inline void IdentityMatrix(XMFLOAT4X4& a)
{
	XMStoreFloat4x4(&a, XMMatrixIdentity());
}

inline void InverseMatrix(XMFLOAT4X4& a)
{
	XMStoreFloat4x4(&a, XMMatrixInverse(NULL, XMLoadFloat4x4(&a)));
}

inline void ScaleMatrix(XMFLOAT4X4& a, float fScale)
{
	XMStoreFloat4x4(&a, XMMatrixScaling(fScale, fScale, fScale) * XMLoadFloat4x4(&a));
}

inline void TranslateLocalX(XMFLOAT4X4& a, float fDistance)
{
	XMStoreFloat4x4(&a, XMMatrixTranslation(fDistance, 0, 0) * XMLoadFloat4x4(&a));
}

inline void TranslateGlobalX(XMFLOAT4X4& a, float fDistance)
{
	XMStoreFloat4x4(&a, XMLoadFloat4x4(&a) * XMMatrixTranslation(fDistance, 0, 0));
}

inline void TranslateLocalY(XMFLOAT4X4& a, float fDistance)
{
	XMStoreFloat4x4(&a, XMMatrixTranslation(0, fDistance, 0) * XMLoadFloat4x4(&a));
}

inline void TranslateGlobalY(XMFLOAT4X4& a, float fDistance)
{
	XMStoreFloat4x4(&a, XMLoadFloat4x4(&a) * XMMatrixTranslation(0, fDistance, 0));
}

inline void TranslateLocalZ(XMFLOAT4X4& a, float fDistance)
{
	XMStoreFloat4x4(&a, XMMatrixTranslation(0, 0, fDistance) * XMLoadFloat4x4(&a));
}

inline void TranslateGlobalZ(XMFLOAT4X4& a, float fDistance)
{
	XMStoreFloat4x4(&a, XMLoadFloat4x4(&a) * XMMatrixTranslation(0, 0, fDistance));
}

inline void RotateLocalX(XMFLOAT4X4& a, float fAngle)
{
	fAngle = RADIANS(fAngle);

	XMStoreFloat4x4(&a, XMMatrixRotationX(fAngle) * XMLoadFloat4x4(&a));
}

inline void RotateGlobalX(XMFLOAT4X4& a, float fAngle)
{
	fAngle = RADIANS(fAngle);

	XMStoreFloat4x4(&a, XMLoadFloat4x4(&a) * XMMatrixRotationX(fAngle));
}

inline void RotateLocalY(XMFLOAT4X4& a, float fAngle)
{
	fAngle = RADIANS(fAngle);

	XMStoreFloat4x4(&a, XMMatrixRotationY(fAngle) * XMLoadFloat4x4(&a));
}

inline void RotateGlobalY(XMFLOAT4X4& a, float fAngle)
{
	fAngle = RADIANS(fAngle);

	XMStoreFloat4x4(&a, XMLoadFloat4x4(&a) * XMMatrixRotationY(fAngle));
}

inline void RotateLocalZ(XMFLOAT4X4& a, float fAngle)
{
	fAngle = RADIANS(fAngle);

	XMStoreFloat4x4(&a, XMMatrixRotationZ(fAngle) * XMLoadFloat4x4(&a));
}

inline void RotateGlobalZ(XMFLOAT4X4& a, float fAngle)
{
	fAngle = RADIANS(fAngle);

	XMStoreFloat4x4(&a, XMLoadFloat4x4(&a) * XMMatrixRotationZ(fAngle));
}

inline void SnapPosition(XMFLOAT4X4& a, XMFLOAT3& b)
{
	a.m[3][0] = b.x;
	a.m[3][1] = b.y;
	a.m[3][2] = b.z;
}

inline void SnapPosition(XMFLOAT4X4& a, XMFLOAT4X4& b)
{
	a.m[3][0] = b.m[3][0];
	a.m[3][1] = b.m[3][1];
	a.m[3][2] = b.m[3][2];
}

inline void SnapRotation(XMFLOAT4X4& a, XMFLOAT4X4& b)
{
	a.m[0][0] = b.m[0][0];
	a.m[0][1] = b.m[0][1];
	a.m[0][2] = b.m[0][2];

	a.m[1][0] = b.m[1][0];
	a.m[1][1] = b.m[1][1];
	a.m[1][2] = b.m[1][2];

	a.m[2][0] = b.m[2][0];
	a.m[2][1] = b.m[2][1];
	a.m[2][2] = b.m[2][2];
}

inline void CreateProjectionMatrix(float fNearPlane, float fFarPlane, XMFLOAT4X4& fProjectionMatrix)
{
	float yScale = 1 / (tan(RADIANS(FOV) / 2));
	float xScale = yScale / RATIO;

	fProjectionMatrix = 
	{
		xScale,		 0,													   0, 0,
			 0, yScale,													   0, 0,
			 0,		 0,				  (fFarPlane / (fFarPlane - fNearPlane)), 1,
			 0,		 0, -(fFarPlane * fNearPlane) / (fFarPlane - fNearPlane), 0
	};
}