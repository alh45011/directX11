#include "Triangle.h"

#include "../ErrorLogger.h"

bool Triangle::Initialize(ID3D11Device* device)
{
	Vertex v[] =
	{
		Vertex(-0.5f,  -0.5f, 1.0f, 0.0f, 0.0f),
		Vertex(0.0f,   0.5f, 1.0f, 0.0f, 1.0f),
		Vertex(0.5f,   -0.5f, 1.0f, 1.0f, 0.0f),
		//Vertex(-0.5f,  -0.5f, 1.0f, 0.0f, 1.0f), //Bottom Left   - [0]
		//Vertex(-0.5f,   0.5f, 1.0f, 0.0f, 0.0f), //Top Left      - [1]
		//Vertex(0.5f,   0.5f, 1.0f, 1.0f, 0.0f), //Top Right     - [2]
		//Vertex(0.5f,  -0.5f, 1.0f, 1.0f, 1.0f), //Bottom Right   - [3]
	};

	HRESULT hr = this->vertexBuffer.Initialize(device, v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	DWORD indices[] =
	{
		0, 1, 2,
	};

	hr = this->indicesBuffer.Initialize(device, indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return false;
	}

	hr = DirectX::CreateWICTextureFromFile(device, L"Data\\Textures\\yellow_square.jpg", nullptr, myTexture.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file.");
		return false;
	}

	return true;
}

void Triangle::Draw(ID3D11DeviceContext* deviceContext)
{
	UINT offset = 0;

	//deviceContext->VSSetConstantBuffers(0, 1, constantBufferGetAddressOf());
	deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf()); // Texture to shader
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(indicesBuffer.BufferSize(), 0, 0);
}