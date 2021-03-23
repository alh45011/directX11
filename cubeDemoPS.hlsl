struct PS_INPUT
{
	float4 inPosition : SV_POSITION;
	float3 PosW : POSITION;
	float3 inNormal : NORMAL;
	float2 Tex : TEXCOORD;
};

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

void ComputeDirectionalLight(Material mat, DirectionalLight L,
                             float3 normal, float3 toEye,
					         out float4 ambient,
						     out float4 diffuse,
						     out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.Direction;

	// Add ambient term.
	ambient = mat.Ambient * L.Ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
					
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}
}

cbuffer cbPerFrame : register(b1)
{
	DirectionalLight gDirLights[3];
	Material gMaterial;
	float3 gEyePosW;
	int gLightCount;
};

Texture2D objTexture : TEXTURE : register(t0);

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	//Filter = MIN_MAG_MIP_LINEAR;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

float4 main(PS_INPUT pin) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
	pin.inNormal = normalize(pin.inNormal);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;
	
	// Default to multiplicative identity.
	float4 texColor = float4(1, 1, 1, 1);
	
	bool gUseTexture = true;
	
	if (gUseTexture)
	{
		// Sample texture.
		texColor = objTexture.Sample(samAnisotropic, pin.Tex);
	}
	
	//
	// Lighting.
	//	
	float4 litColor = texColor;
	if (gLightCount > 0)
	{
		// Start with a sum of zero. 
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Sum the light contribution from each light source.  
		[unroll]
		for (int i = 0; i < gLightCount; ++i)
		{
			float4 A, D, S;
			ComputeDirectionalLight(gMaterial, gDirLights[i], pin.inNormal, toEye,
				A, D, S);

			ambient += A;
			diffuse += D;
			spec += S;
		}

		// Modulate with late add.
		litColor = texColor * (ambient + diffuse) + spec;
	}

	// Common to take alpha from diffuse material and texture.
	litColor.a = gMaterial.Diffuse.a * texColor.a;

	return litColor;
}