#define AMBIENT 0.20f
#define SPECULAR_AMOUNT 1.5f
#define SPECULAR_POWER 128.0f

struct PixelIn
{
	float4 m_fOldCoordinates : OLD_POSITION;
	float4 m_fCoordinates : SV_POSITION;
	float4 m_fTexCoords : TEXCOORD0;
	float4 m_fNormals : NORMALS;
	float4 m_fTangents : TANGENTS;
};

struct DirectionalLight
{
	float4 m_fDirection;
	float4 m_fColor;
};

struct PointLight
{
	float4 m_fPosition;
	float4 m_fColor;
	float m_fRadius;
	float m_fBrightness;
	float2 m_fPadding;
};

cbuffer bDLightBuffer : register(b6)
{
	DirectionalLight tDLight;
}

cbuffer bLightBuffer : register(b7)
{
	PointLight tLights[NUM_LIGHTS + 1];
}

cbuffer bCameraBuffer : register(b8)
{
	float4 fCameraPos;
}

inline float3 CalcPointLight(float3 fLightPos, float3 fPixelPos, float3 fNormal, float fRadius, float3 fColor, float3 fDiffuse, float fSpecular)
{
	float3 fPixelToLight = fLightPos - fPixelPos;
	float3 fLightDir = normalize(fPixelToLight);
	float fIntensity = saturate(dot(fLightDir, fNormal));
	float fAttenuation = 1.0f - saturate(length(fPixelToLight) / fRadius);
	fIntensity *= fAttenuation * fAttenuation;

	// Specular //
	float3 fSpecFinal = {0, 0, 0};
	if (fIntensity > 0.0001f)
	{
		float3 fViewDir = normalize(fCameraPos.xyz - fPixelPos);
		float3 fHalfVec = normalize(fLightDir + fViewDir);
		float fSpecIntensity = pow(saturate(dot(fNormal, fHalfVec)), SPECULAR_POWER);

		fSpecFinal = fColor * fSpecIntensity * fAttenuation * fDiffuse * (fSpecular + 0.5f);
	}

	return (fIntensity * fColor * fDiffuse) + fSpecFinal;
}

inline float3 CalcDirectLight(float3 fLightDir, float3 fNormal, float3 fDiffuse, float3 fColor)
{
	float fIntensity = saturate(dot(normalize(-fLightDir), fNormal));
	return (fIntensity * fColor * fDiffuse);
}

inline float4 ComputeLighting(PixelIn cPixel, float4 fDiffuse, float3 fNormal, float fSpecular)
{
	float3 fTotalLight = { 0, 0, 0 };
	float3 fPointLight = { 0, 0, 0 };
	float3 fDirectLight = { 0, 0, 0 };
	
	// Point Lights //
	[unroll]
	for (int i = 0; i < NUM_LIGHTS; i++)
		fPointLight += CalcPointLight(tLights[i].m_fPosition.xyz, cPixel.m_fOldCoordinates.xyz, fNormal.xyz, tLights[i].m_fRadius, tLights[i].m_fColor.xyz * tLights[i].m_fBrightness, fDiffuse.xyz, fSpecular);

	// Directional Light //
	fDirectLight += CalcDirectLight(tDLight.m_fDirection.xyz, fNormal.xyz, fDiffuse.xyz, tDLight.m_fColor.xyz);

	fTotalLight = fPointLight + fDirectLight + (AMBIENT * fDiffuse.xyz); // Ambient applied after everything. Makes entire scene brighter.

	return float4(fTotalLight, fDiffuse.w);
}