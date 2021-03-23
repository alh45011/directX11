#include "DefaultDemo.h"

vector<D3D11_INPUT_ELEMENT_DESC> DefaultDemo::BuildVertexLayout()
{
	// Create the vertex input layout.
	vector<D3D11_INPUT_ELEMENT_DESC> vertexDesc =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	return vertexDesc;
}

bool DefaultDemo::InitializeScene(GraphicsTools graphicsTools)
{
	////Textured Square
	//Vertex v[] =
	//{
	//	Vertex(-0.5f,  -0.5f, 0.0f, 0.0f, 1.0f), //Bottom Left   - [0]
	//	Vertex(-0.5f,   0.5f, 0.0f, 0.0f, 0.0f), //Top Left      - [1]
	//	Vertex(0.5f,   0.5f, 0.0f, 1.0f, 0.0f), //Top Right     - [2]
	//	Vertex(0.5f,  -0.5f, 0.0f, 1.0f, 1.0f), //Bottom Right   - [3]
	//};

	////Load Vertex Data
	//HRESULT hr = this->vertexBuffer.Initialize(graphicsTools.device, v, ARRAYSIZE(v));
	//if (FAILED(hr))
	//{
	//	ErrorLogger::Log(hr, "Failed to create vertex buffer.");
	//	return false;
	//}

	//DWORD indices[] =
	//{
	//	0, 1, 2,
	//	0, 2, 3
	//};

	////Load Index Data

	//hr = this->indicesBuffer.Initialize(this->device.Get(), indices, ARRAYSIZE(indices));
	//if (FAILED(hr))
	//{
	//	ErrorLogger::Log(hr, "Failed to create indices buffer.");
	//	return hr;
	//}

	////Load Texture
	//hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\piano.png", nullptr, myTexture.GetAddressOf());
	//if (FAILED(hr))
	//{
	//	ErrorLogger::Log(hr, "Failed to create wic texture from file.");
	//	return false;
	//}

	////Initialize Constant Buffer(s)
	//hr = this->constantBuffer.Initialize(this->device.Get(), this->deviceContext.Get());
	//if (FAILED(hr))
	//{
	//	ErrorLogger::Log(hr, "Failed to initialize constant buffer.");
	//	return false;
	//}

	//camera.SetPosition(0.0f, 0.0f, -2.0f);
	//camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);

	return true;
}

bool Init(ID3D11Device* device)
{
	return true;
}