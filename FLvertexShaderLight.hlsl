struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w = SpecPower
	float4 Reflect;
};

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float pad;
};

struct PointLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Position;
	float Range;
	float3 Att;
	float pad;
};

struct SpotLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Position;
	float Range;
	float3 Direction;
	float Spot;
	float3 Att;
	float pad;
};

cbuffer mycBuffer : register(b0)
{
	float4x4 gWorldViewProj;
}

cbuffer mycBuffer1 : register(b1)
{	
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;	
	Material gMaterial;
}; 

cbuffer cbPerFrame : register(b2)
{
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	float3 gEyePosW;
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
	
	float4 outA : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	output.outPosition = mul(float4(input.inPos, 1.0f), gWorldViewProj);
	output.PosW = mul(float4(input.inPos, 1.0f), gWorld).xyz;
	output.outNormal = mul(input.inNormal, (float3x3) gWorldInvTranspose);
	
	return output;
}