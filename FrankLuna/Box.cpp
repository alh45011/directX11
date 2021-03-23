#include "Box.h"

bool Box::Init(ID3D11Device* device)
{
	timer.Start();

	if (!BuildGeometryBuffers(device))
		return false;
	
	BuildVertexLayout();

	return true;
}

bool Box::BuildGeometryBuffers(ID3D11Device* device)
{
	// Create vertex buffer
	std::vector<VertexFL> vertices =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), ColorUtils::FloatColorToDXColor(ColorsFL::White)   },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), ColorUtils::FloatColorToDXColor(ColorsFL::Black)   },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), ColorUtils::FloatColorToDXColor(ColorsFL::Red)     },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), ColorUtils::FloatColorToDXColor(ColorsFL::Green)   },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), ColorUtils::FloatColorToDXColor(ColorsFL::Blue)    },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), ColorUtils::FloatColorToDXColor(ColorsFL::Yellow)  },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), ColorUtils::FloatColorToDXColor(ColorsFL::Cyan)    },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), ColorUtils::FloatColorToDXColor(ColorsFL::Magenta) }
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

void Box::DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer)
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


	ConstantBuffer<Time_vertexshader> constantBuffer_Timer;
	constantBuffer_Timer.Initialize(graphicsTools.device, graphicsTools.deviceContext);
	constantBuffer_Timer.data.gTime = timer.GetMilisecondsElapsed() / 5000.0f;
	constantBuffer_Timer.ApplyChanges();
	
	graphicsTools.deviceContext->VSSetConstantBuffers(1, 1, constantBuffer_Timer.GetAddressOf());

	graphicsTools.deviceContext->DrawIndexed(this->indicesBuffer.BufferSize(), 0, 0);
}