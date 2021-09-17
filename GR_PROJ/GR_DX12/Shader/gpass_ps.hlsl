struct VS_OUT
{
	float4 worldPosition:SV_POSITION;
	float4 color:COLOR0;
	float3 normal:NORMAL0;
};

struct PS_OUT
{
	float4 worldPosition:SV_TARGET0;
	//float3 normal:SV_TARGET1;
	//float3 color:SV_TARGET2;
};

float4 main(VS_OUT i) : SV_TARGET
{
	PS_OUT o;
	o.worldPosition = i.worldPosition;
	//o.normal = i.normal;
	//o.color = i.color;
	return float4(1, 1, 0, 1);
}