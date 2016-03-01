texture2D tTexture : register(t0);
SamplerState sSamplerState : register(s0);

struct PixelInput
{
	float4 m_fCoordinates : SV_POSITION;
	float4 m_fTexCoords : TEXCOORD0;
};

float4 main(PixelInput tPixelIn) : SV_TARGET
{
	return float4(0, 0, 0, 0);
}