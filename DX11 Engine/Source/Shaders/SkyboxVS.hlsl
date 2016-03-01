#pragma pack_matrix(row_major)

struct VertexIn
{
	float4 m_fCoordinates : POSITIONS;
	float4 m_fTexCoords : TEXCOORDS;
	float4 m_fNormals : NORMALS;
	float4 m_fTangets : TANGENTS;
};

struct VertexOut
{
	float4 m_fOldCoordinates : OLD_POSITION;
	float4 m_fCoordinates : SV_POSITION;
	float4 m_fTexCoords : TEXCOORD0;
	float4 m_fNormals : NORMALS;
};

cbuffer bObject : register(b0)
{
	float4x4 fTransformMatrix;
}

cbuffer bView : register(b1)
{
	float4x4 fViewMatrix;
}

cbuffer bProjection : register(b2)
{
	float4x4 fProjectionMatrix;
}

VertexOut main(VertexIn cVertexIn)
{
	VertexOut cVertexOut;

	float4 fLocalPosition = float4(cVertexIn.m_fCoordinates.xyz, 1);
	float4x4 fNormalTransform = fTransformMatrix;
	fNormalTransform[3] = float4(0, 0, 0, 0); // Zero out position	

	// Perspective Divide //
	fLocalPosition = mul(fLocalPosition, fTransformMatrix);
	fLocalPosition = mul(fLocalPosition, fViewMatrix);
	fLocalPosition = mul(fLocalPosition, fProjectionMatrix);

	cVertexOut.m_fOldCoordinates = cVertexIn.m_fCoordinates;
	cVertexOut.m_fCoordinates = fLocalPosition;
	cVertexOut.m_fTexCoords = cVertexIn.m_fTexCoords;
	cVertexOut.m_fNormals = mul(cVertexIn.m_fNormals, fNormalTransform);

	return cVertexOut;
}