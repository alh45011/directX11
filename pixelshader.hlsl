cbuffer alphaBuffer : register(b0)
{
	float alpha;
}

struct PS_INPUT
{
	float4 inPosition : SV_POSITION;
	//float3 inColor : COLOR;
	float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	//return float4(input.inColor, 1.0f);
	float3 pixelColor = objTexture.Sample(objSamplerState, input.inTexCoord);
	return float4(pixelColor, alpha);
}