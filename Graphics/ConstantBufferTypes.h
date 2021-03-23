#pragma once
#include <DirectXMath.h>
#include "../FrankLuna/Light.h"

struct CB_VS_vertexshader
{
	DirectX::XMMATRIX mat;
};

struct CB_PS_pixelshader
{
	float alpha = 1.0f;
};

struct Time_vertexshader
{
	float gTime;
};

// Chapter 7 buffers
struct FL_Light_Projections
{
	DirectX::XMMATRIX gWorld;
	DirectX::XMMATRIX gWorldInvTranspose;
	Material gMaterial;
};

struct FL_Light_Per_Frame
{
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	DirectX::XMFLOAT3 gEyePosW;
};

struct CH7_Projections
{
	DirectX::XMMATRIX gWorldViewProj;
	DirectX::XMMATRIX gWorld;
	DirectX::XMMATRIX gWorldInvTranspose;
};

struct CH7_LightPerFrame
{
	DirectionalLight gDirLights[3];
	Material gMaterial;
	DirectX::XMFLOAT3 gEyePosW;
	//DirectX::XMFLOAT1 pad;
	int gLightCount;
};

struct CH8_ProjectionsTexture
{
	DirectX::XMMATRIX gWorldViewProj;
	DirectX::XMMATRIX gWorld;
	DirectX::XMMATRIX gWorldInvTranspose;
	DirectX::XMMATRIX gTexTransform;
};