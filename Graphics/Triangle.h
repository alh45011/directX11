#pragma once

#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

#include <WICTextureLoader.h>

class Triangle
{
public:
	bool Initialize(ID3D11Device* device);
	void Draw(ID3D11DeviceContext* deviceContext);

//private:
	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indicesBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myTexture;
};