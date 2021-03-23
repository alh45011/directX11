cbuffer mycBuffer : register(b0)
{
	float4x4 gWorldViewProj;
}; 

cbuffer mycBuffer2 : register(b1)
{
	float gTime;
}; 

struct VS_INPUT
{
	float3 inPos : POSITION;
	float3 inColor : COLOR;
};

struct VS_OUTPUT
{
	float4 outPosition : SV_POSITION;
	float3 outColor : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	input.inPos.xy += 0.5f * sin(input.inPos.x) * sin(3.0f * gTime);
	input.inPos.z *= 0.6f + 0.4f * sin(2.0f * gTime);
	
	output.outPosition = mul(float4(input.inPos, 1.0f), gWorldViewProj);
	output.outColor = input.inColor;
	return output;
}