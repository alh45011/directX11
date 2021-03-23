cbuffer mycBuffer : register(b0)
{
	float4x4 gWorldViewProj;
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gTexTransform;
}; 

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;

struct VS_INPUT
{
	float3 inPos : POSITION;
	float3 inNormal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 outPosition : SV_POSITION;
	float3 PosW : POSITION;
	float3 outNormal : NORMAL;
	float2 Tex : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	// Transform to world space space.	
	output.PosW = mul(float4(input.inPos, 1.0f), gWorld).xyz;
	output.outNormal = mul(input.inNormal, (float3x3) gWorldInvTranspose);
	
	// Transform to homogeneous clip space.
	output.outPosition = mul(float4(input.inPos, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	output.Tex = mul(float4(input.Tex, 0.0f, 1.0f), gTexTransform).xy;
	
	return output;
}