cbuffer mycBuffer : register(b0)
{
	float4x4 gWorldViewProj;
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
}; 

struct VS_INPUT
{
	float3 inPos : POSITION;
	float3 inNormal : NORMAL;
};

struct VS_OUTPUT
{
	float4 outPosition : SV_POSITION;
	float3 PosW : POSITION;
	float3 outNormal : NORMAL;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	output.outPosition = mul(float4(input.inPos, 1.0f), gWorldViewProj);
	output.PosW = mul(float4(input.inPos, 1.0f), gWorld).xyz;
	output.outNormal = mul(input.inNormal, (float3x3) gWorldInvTranspose);
	
	return output;
}