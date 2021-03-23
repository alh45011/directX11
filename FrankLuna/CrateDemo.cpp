#include "CrateDemo.h"

vector<D3D11_INPUT_ELEMENT_DESC> CrateDemo::BuildVertexLayout()
{
	// Create the vertex input layout.
	vector<D3D11_INPUT_ELEMENT_DESC> vertexDesc =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	return vertexDesc;
}

bool CrateDemo::BuildGeometryBuffers(ID3D11Device* device)
{
	GeometryGenerator::MeshData box;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset = 0;

	// Cache the index count of each object.
	mBoxIndexCount = box.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset = 0;

	UINT totalVertexCount = box.Vertices.size();

	UINT totalIndexCount = mBoxIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<vertexPosNormalTex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex = box.Vertices[i].TexC;
	}

	HRESULT hr = this->mBoxVB.Initialize(device, vertices.data(), vertices.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	//
	// Pack the indices of all the meshes into one index buffer.
	//
	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());

	hr = this->mBoxIB.Initialize(device, indices.data(), indices.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return hr;
	}

	return true;
}

void CrateDemo::DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer)
{
	graphicsTools.deviceContext->ClearRenderTargetView(graphicsTools.renderTargetView, ColorsFL::LightSteelBlue);
	graphicsTools.deviceContext->ClearDepthStencilView(graphicsTools.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	graphicsTools.deviceContext->IASetInputLayout(graphicsTools.vertexshader.GetInputLayout());
	graphicsTools.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CreateRasterizerState(graphicsTools);

	UINT stride = sizeof(vertexPosNormalTex);
	UINT offset = 0;
	graphicsTools.deviceContext->IASetVertexBuffers(0, 1, mBoxVB.GetAddressOf(), &stride, &offset);
	graphicsTools.deviceContext->IASetIndexBuffer(mBoxIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	graphicsTools.deviceContext->OMSetDepthStencilState(graphicsTools.depthStencilState, 0);
	graphicsTools.deviceContext->VSSetShader(graphicsTools.vertexshader.GetShader(), NULL, 0);
	graphicsTools.deviceContext->PSSetShader(graphicsTools.pixelshader.GetShader(), NULL, 0);
	graphicsTools.deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf()); // Texture to shader

	// Draw the model.
	XMMATRIX world = XMLoadFloat4x4(&mBoxWorld);

	ConstantBuffer<CH8_ProjectionsTexture> constBuf;
	constBuf.Initialize(graphicsTools.device, graphicsTools.deviceContext);
	constBuf.data.gWorldViewProj = world * graphicsTools.view * graphicsTools.proj;
	constBuf.data.gWorldViewProj = DirectX::XMMatrixTranspose(constBuf.data.gWorldViewProj);
	constBuf.data.gWorld = world;
	constBuf.data.gWorldInvTranspose = MathHelper::InverseTranspose(world);
	constBuf.data.gTexTransform = XMLoadFloat4x4(&mTexTransform);
	if (!constBuf.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constBuf.GetAddressOf());

	ConstantBuffer<CH7_LightPerFrame> cbLightPerFrame;
	cbLightPerFrame.Initialize(graphicsTools.device, graphicsTools.deviceContext);
	cbLightPerFrame.data.gDirLights[0] = mDirLights[0];
	cbLightPerFrame.data.gDirLights[1] = mDirLights[1];
	cbLightPerFrame.data.gDirLights[2] = mDirLights[2];
	cbLightPerFrame.data.gEyePosW = graphicsTools.cameraPosition;
	cbLightPerFrame.data.gMaterial = mBoxMat;
	cbLightPerFrame.data.gLightCount = 3;
	if (!cbLightPerFrame.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->PSSetConstantBuffers(1, 1, cbLightPerFrame.GetAddressOf());

	graphicsTools.deviceContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);	
}

bool CrateDemo::Init(ID3D11Device* device)
{
	timer.Start();

	if (!BuildGeometryBuffers(device))
		return false;

	BuildVertexLayout();

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mBoxWorld, I);
	XMStoreFloat4x4(&mTexTransform, I);

	mDirLights[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[1].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(1.4f, 1.4f, 1.4f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.707f, 0.0f, 0.707f);

	mBoxMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mBoxMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);

	mEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);

	HRESULT hr = DirectX::CreateWICTextureFromFile(device, L"Textures\\WoodCrate01.dds", nullptr, myTexture.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file.");
		return false;
	}

	return true;
}