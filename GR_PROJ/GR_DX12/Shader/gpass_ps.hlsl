struct VS_OUT
{
	float4 worldPosition:SV_POSITION;
	float2 uv : TEXCOORD0;
};

struct PS_OUT
{
	float4 worldPosition:SV_TARGET0;
};

Texture2D diffuse : register(t1);
SamplerState defaultSampler : register(s0);

float4 main(VS_OUT i) : SV_TARGET
{
	return diffuse.Sample(defaultSampler,i.uv);
}