
struct VS_IN
{
	float3 modelPosition:POSITION0;
	//float4 color:COLOR0;
	//float3 normal:NORMAL0;
};

struct VS_OUT
{
	float4 worldPosition:SV_POSITION;
	//float4 color:COLOR0;
	//float3 normal:NORMAL0;
};

//cbuffer matrixInfo : register(b0)
//{
//	float4x4 viewMat;
//	float4x4 projMat;
//};

VS_OUT main(VS_IN i)
{
	VS_OUT o;
	o.worldPosition = float4(i.modelPosition.xyz, 1.0);
	//o.worldPosition = mul(projMat, mul(viewMat, float4(i.modelPosition.xyz, 1.0)));
	//o.normal = i.normal;
	//o.color = i.color;

	return o;
}