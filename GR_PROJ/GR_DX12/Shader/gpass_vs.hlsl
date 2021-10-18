
struct VS_IN
{
	float3 modelPosition:POSITION0;
	float2 uv : TEXCOORD0;
};

struct VS_OUT
{
	float4 worldPosition:SV_POSITION;
	float2 uv0 : TEXCOORD0;
};

cbuffer HHQ_MATRIX_V : register(b0)
{
	float4x4 viewMatrix;
}

cbuffer HHQ_MATRIX_P : register(b1)
{
	float4x4 projMatrix;
}

cbuffer HHQ_MATRIX_Model : register(b2)
{
	float4x4 modelMatrix;
}

VS_OUT main(VS_IN i)
{
	VS_OUT o;
	o.worldPosition = mul(projMatrix, mul(viewMatrix, mul(modelMatrix,float4(i.modelPosition.xyz, 1.0))));
	o.uv0 = i.uv;

	return o;
}