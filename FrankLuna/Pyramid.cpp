#include "Pyramid.h"

bool Pyramid::Init(ID3D11Device* device)
{
	if (!BuildGeometryBuffers(device))
		return false;

	BuildVertexLayout();

	return true;
}

bool Pyramid::BuildGeometryBuffers(ID3D11Device* device)
{
	// Create vertex buffer
	std::vector<VertexFL> vertices =
	{
		{ XMFLOAT3(-1.0f, 0.0f, -1.0f), ColorUtils::FloatColorToDXColor(ColorsFL::Green)   }, // 0
		{ XMFLOAT3(1.0f,  0.0f, -1.0f), ColorUtils::FloatColorToDXColor(ColorsFL::Green)   }, // 1
		{ XMFLOAT3(-1.0f, 0.0f, 1.0f), ColorUtils::FloatColorToDXColor(ColorsFL::Green)     },// 2
		{ XMFLOAT3( 1.0f, 0.0f, 1.0f), ColorUtils::FloatColorToDXColor(ColorsFL::Green)   },  // 3
		{ XMFLOAT3(0.0f, 3.0f, 0.0f), ColorUtils::FloatColorToDXColor(ColorsFL::Red)    },    // 4
	};

	HRESULT hr = this->vertexBuffer.Initialize(device, vertices.data(), vertices.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	// Create the index buffer

	std::vector<UINT> indices = {
		// front face
		0, 4, 1,
		1,4,3,
		
		// left face
		3, 4, 2,
		0, 2, 4,
		0, 3, 2,
		0, 1, 3
		
	};

	hr = this->indicesBuffer.Initialize(device, indices.data(), indices.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return false;
	}

	return true;
}

void Pyramid::DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer)
{
	graphicsTools.deviceContext->ClearRenderTargetView(graphicsTools.renderTargetView, ColorsFL::LightSteelBlue);
	graphicsTools.deviceContext->ClearDepthStencilView(graphicsTools.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	graphicsTools.deviceContext->IASetInputLayout(graphicsTools.vertexshader.GetInputLayout());
	graphicsTools.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(VertexFL);
	UINT offset = 0;
	graphicsTools.deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	graphicsTools.deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//graphicsTools.deviceContext->RSSetState(graphicsTools.rasterizerState); // FIX ME !
	graphicsTools.deviceContext->OMSetDepthStencilState(graphicsTools.depthStencilState, 0);
	//graphicsTools.deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	graphicsTools.deviceContext->VSSetShader(graphicsTools.vertexshader.GetShader(), NULL, 0);
	graphicsTools.deviceContext->PSSetShader(graphicsTools.pixelshader.GetShader(), NULL, 0);

	//this->deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf()); // Texture to shader
	graphicsTools.deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
	graphicsTools.deviceContext->IASetIndexBuffer(this->indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	XMMATRIX world = XMMatrixIdentity();

	constantBuffer.data.mat = world * graphicsTools.view * graphicsTools.proj;
	constantBuffer.data.mat = DirectX::XMMatrixTranspose(constantBuffer.data.mat);
	if (!constantBuffer.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	graphicsTools.deviceContext->DrawIndexed(this->indicesBuffer.BufferSize(), 0, 0);
}