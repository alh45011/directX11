#pragma once

#include <DirectXMath.h>

struct VertexFL
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct vertexPosNormal
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
};

struct vertexPosNormalTex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
};

namespace ColorsFL
{
	const float White[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float Black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float Red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	const float Green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	const float Blue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	const float Yellow[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	const float Cyan[] = { 0.0f, 1.0f, 1.0f, 1.0f };
	const float Magenta[] = { 1.0f, 0.0f, 1.0f, 1.0f };

	const float Silver[] = { 0.75f, 0.75f, 0.75f, 1.0f };
	const float LightSteelBlue[] = { 0.69f, 0.77f, 0.87f, 1.0f };
	
};

struct ColorUtils
{
	static XMFLOAT4 FloatColorToDXColor(const float color[4])
	{
		return XMFLOAT4(color[0], color[1], color[2], color[3]);
	}
};