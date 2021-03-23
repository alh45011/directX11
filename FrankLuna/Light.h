#pragma once

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float Pad; // Pad the last float so we can
	// array of lights if we wanted.
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 Position;
	float Range;
	// Packed into 4D vector: (A0, A1, A2, Pad)
	XMFLOAT3 Att;
	float Pad; // Pad the last float so we can set an
	// array of lights if we wanted.
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 Position;
	float Range;
	// Packed into 4D vector: (Direction,
	XMFLOAT3 SpotDirection;
	float Spot;
	// Packed into 4D vector: (Att, Pad)
	XMFLOAT3 Attenuation;
	float Pad; // Pad the last float so we can set an
	// array of lights if we wanted.
};

struct Material
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;
};