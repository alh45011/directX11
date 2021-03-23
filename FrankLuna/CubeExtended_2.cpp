#include "CubeExtended_2.h"

vector<D3D11_INPUT_ELEMENT_DESC> CubeExtended_2::BuildVertexLayout()
{
	// Create the vertex input layout.
	vector<D3D11_INPUT_ELEMENT_DESC> vertexDesc =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	return vertexDesc;
}

bool CubeExtended_2::Init(ID3D11Device* device)
{
	if (!BuildGeometryBuffers(device))
		return false;

	BuildVertexLayout();

	return true;
}

bool CubeExtended_2::BuildGeometryBuffers(ID3D11Device* device)
{
	std::vector< Cube02FL_Pos> positions =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f)},
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f)},
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f)},
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f)},
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f)},
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f)},
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f)},
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f)}
	};
	

	HRESULT hr = this->vertexBufferOne.Initialize(device, positions.data(), positions.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	std::vector<Cube02FL_Color> colors = 
	{
		{ColorUtils::FloatColorToDXColor(ColorsFL::White)   },
		{ColorUtils::FloatColorToDXColor(ColorsFL::Black)   },
		{ColorUtils::FloatColorToDXColor(ColorsFL::Red)     },
		{ColorUtils::FloatColorToDXColor(ColorsFL::Green)   },
		{ColorUtils::FloatColorToDXColor(ColorsFL::Blue)    },
		{ColorUtils::FloatColorToDXColor(ColorsFL::Yellow)  },
		{ColorUtils::FloatColorToDXColor(ColorsFL::Cyan)    },
		{ColorUtils::FloatColorToDXColor(ColorsFL::Magenta) }
	};

	hr = this->vertexBufferTwo.Initialize(device, colors.data(), colors.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	// Create the index buffer

	std::vector<UINT> indices = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	hr = this->indicesBuffer.Initialize(device, indices.data(), indices.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return false;
	}

	return true;
}

void CubeExtended_2::DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer)
{
	graphicsTools.deviceContext->ClearRenderTargetView(graphicsTools.renderTargetView, ColorsFL::LightSteelBlue);
	graphicsTools.deviceContext->ClearDepthStencilView(graphicsTools.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	graphicsTools.deviceContext->IASetInputLayout(graphicsTools.vertexshader.GetInputLayout());
	graphicsTools.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Cube02FL_Pos);
	UINT offset = 0;
	graphicsTools.deviceContext->IASetVertexBuffers(0, 1, vertexBufferOne.GetAddressOf(), &stride, &offset);

	stride = sizeof(Cube02FL_Color);
	offset = 0;
	graphicsTools.deviceContext->IASetVertexBuffers(1, 1, vertexBufferTwo.GetAddressOf(), &stride, &offset);

	graphicsTools.deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//graphicsTools.deviceContext->RSSetState(graphicsTools.rasterizerState); // FIX ME !
	graphicsTools.deviceContext->OMSetDepthStencilState(graphicsTools.depthStencilState, 0);
	//graphicsTools.deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	graphicsTools.deviceContext->VSSetShader(graphicsTools.vertexshader.GetShader(), NULL, 0);
	graphicsTools.deviceContext->PSSetShader(graphicsTools.pixelshader.GetShader(), NULL, 0);

	//this->deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf()); // Texture to shader
	/*graphicsTools.deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
	graphicsTools.deviceContext->IASetIndexBuffer(this->indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);*/

	XMMATRIX world = XMMatrixIdentity();

	constantBuffer.data.mat = world * graphicsTools.view * graphicsTools.proj;
	constantBuffer.data.mat = DirectX::XMMatrixTranspose(constantBuffer.data.mat);
	if (!constantBuffer.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	graphicsTools.deviceContext->DrawIndexed(this->indicesBuffer.BufferSize(), 0, 0);
}