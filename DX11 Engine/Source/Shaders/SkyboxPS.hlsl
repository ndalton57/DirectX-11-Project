TextureCube tSkybox : register(t0);
SamplerState sSampler : register(s0);

struct PixelIn
{
	float4 m_fOldCoordinates : OLD_POSITION;
	float4 m_fCoordinates : SV_POSITION;
	float4 m_fTexCoords : TEXCOORD0;
	float4 m_fNormals : NORMALS;
};

float4 main(PixelIn cPixelIn) : SV_TARGET
{
	return tSkybox.Sample(sSampler, cPixelIn.m_fOldCoordinates.xyz);
}