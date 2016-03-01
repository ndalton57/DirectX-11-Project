#pragma pack_matrix(row_major)

struct VertexIn
{
	float4 m_fCoordinates : POSITIONS;
	float4 m_fTexCoords : TEXCOORDS;
};

struct VertexOut
{
	float4 m_fCoordinates : SV_POSITION;
	float4 m_fTexCoords : TEXCOORD0;
};

VertexOut main(VertexIn cVertexIn)
{
	VertexOut cVertexOut;

	cVertexOut.m_fCoordinates = cVertexIn.m_fCoordinates;
	cVertexOut.m_fTexCoords = cVertexIn.m_fTexCoords;

	return cVertexOut;
}