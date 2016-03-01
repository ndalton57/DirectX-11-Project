#include "Lighting.hlsli"

texture2D tDiffuseMap : register(t0);
texture2D tNormalMap : register(t1);
texture2D tSpecularMap : register(t2);
SamplerState sSampler : register(s0);

inline float3 ConvertNormals(float2 fTexCoord, float3 fNormal, float3 fTangent)
{
	float3x3 TBN = float3x3(fTangent, cross(fTangent, fNormal), fNormal);

	float3 fNewNormal = tNormalMap.Sample(sSampler, fTexCoord).xyz;
	fNewNormal *= 2.0f;
	fNewNormal -= 1.0f;

	fNewNormal = normalize(mul(fNewNormal, TBN));

	return fNewNormal;
}

float4 main(PixelIn cPixelIn) : SV_TARGET
{
	// OpenGL UV Invert //
	#if OPENGL
		float2 fTexCoord = (cPixelIn.m_fTexCoords.xy - 1.0f) * -1.0f;
	#else
		float2 fTexCoord = cPixelIn.m_fTexCoords.xy;
	#endif

	float4 fDiffuse = tDiffuseMap.Sample(sSampler, fTexCoord);
	float3 fNormal = ConvertNormals(fTexCoord, cPixelIn.m_fNormals.xyz, cPixelIn.m_fTangents.xyz);
	float fSpecular = tSpecularMap.Sample(sSampler, fTexCoord).x;

	float4 fTotalLight = ComputeLighting(cPixelIn, fDiffuse, fNormal, fSpecular);

	return saturate(fTotalLight);
}