#include "TexturedHillsAndWavesDemo.h"

bool TexturedHillsAndWavesDemo::Init(ID3D11Device* device)
{
	timer.Start();

	mWaves.Init(160, 160, 1.0f, 0.03f, 3.25f, 0.4f);

	if (!BuildLandGeometryBuffers(device))
		return false;

	if (!BuildWavesGeometryBuffers(device))
		return false;

	BuildVertexLayout();

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mLandWorld, I);
	XMStoreFloat4x4(&mWavesWorld, I);

	XMMATRIX grassTexScale = XMMatrixScaling(1.0f, 1.0f, 0.0f);
	XMStoreFloat4x4(&mGrassTexTransform, grassTexScale);

	mDirLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	mLandMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mLandMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mLandMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mWavesMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mWavesMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);

	HRESULT hr = DirectX::CreateWICTextureFromFile(device, L"Textures\\Chapter8\\grass.dds", nullptr, mGrassMapSRV.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file.");
		return false;
	}

	hr = DirectX::CreateWICTextureFromFile(device, L"Textures\\Chapter8\\water2.dds", nullptr, mWavesMapSRV.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file.");
		return false;
	}

	return true;
}

float TexturedHillsAndWavesDemo::GetHillHeight(float x, float z) const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

XMFLOAT3 TexturedHillsAndWavesDemo::GetHillNormal(float x, float z)const
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

bool TexturedHillsAndWavesDemo::BuildLandGeometryBuffers(ID3D11Device* device)
{
	GeometryGenerator::MeshData grid;
	GeometryGenerator geoGen;

	geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

	mLandIndexCount = grid.Indices.size();

	//
	// Extract the vertex elements we are interested and apply the height function to
	// each vertex.  In addition, color the vertices based on their height so we have
	// sandy looking beaches, grassy low hills, and snow mountain peaks.
	//

	std::vector<vertexPosNormalTex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		p.y = GetHillHeight(p.x, p.z);

		vertices[i].Pos = p;
		vertices[i].Normal = GetHillNormal(p.x, p.z);
		vertices[i].Tex = grid.Vertices[i].TexC;
	}

	HRESULT hr = this->mLandVB.Initialize(device, vertices.data(), vertices.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	hr = this->mLandIB.Initialize(device, grid.Indices.data(), grid.Indices.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return false;
	}

	return true;
}

bool TexturedHillsAndWavesDemo::BuildWavesGeometryBuffers(ID3D11Device* device)
{
	// Create the vertex buffer.  Note that we allocate space only, as
	// we will be updating the data every time step of the simulation.
	HRESULT hr = this->mWavesVB.Initialize(device, 0, sizeof(vertexPosNormalTex) * mWaves.VertexCount(), "dynamic");
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	// Create the index buffer.  The index buffer is fixed, so we only 
	// need to create and set once.

	std::vector<UINT> indices(3 * mWaves.TriangleCount()); // 3 indices per face

	// Iterate over each quad.
	UINT m = mWaves.RowCount();
	UINT n = mWaves.ColumnCount();
	int k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (DWORD j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}

	hr = this->mWavesIB.Initialize(device, indices.data(), indices.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return hr;
	}

	return true;
}

bool TexturedHillsAndWavesDemo::BuildGeometryBuffers(ID3D11Device* device)
{
	if (!BuildLandGeometryBuffers(device))
		return false;

	if (!BuildWavesGeometryBuffers(device))
		return false;

	return true;
}

void TexturedHillsAndWavesDemo::UpdateScene(GraphicsTools graphicsTools, float dt)
{
	mEyePosW = graphicsTools.cameraPosition;

	//
	// Every quarter second, generate a random wave.
	//
	static float t_base = 0.0f;
	if ((timer.GetMilisecondsElapsed() - t_base) >= 250.0f)
	{
		t_base += 0.25f;

		DWORD i = 5 + rand() % (mWaves.RowCount() - 10);
		DWORD j = 5 + rand() % (mWaves.ColumnCount() - 10);

		float r = MathHelper::randMToN(1.0f, 2.0f);
		//float r = MathHelper::RandF(1.0f, 2.0f);

		mWaves.Disturb(i, j, r);
	}

	//mWaves.Update(dt);

	//
	// Update the wave vertex buffer with the new solution.
	//

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HRESULT hr = graphicsTools.deviceContext->Map(mWavesVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state.");
		exit(-1);
	}

	vertexPosNormalTex* v = reinterpret_cast<vertexPosNormalTex*>(mappedData.pData);
	for (UINT i = 0; i < mWaves.VertexCount(); ++i)
	{
		v[i].Pos = mWaves[i];
		v[i].Normal = mWaves.Normal(i);

		// Derive tex-coords in [0,1] from position.
		v[i].Tex.x = 0.5f + mWaves[i].x / mWaves.Width();
		v[i].Tex.y = 0.5f - mWaves[i].z / mWaves.Depth();
	}

	graphicsTools.deviceContext->Unmap(mWavesVB.Get(), 0);

	//
	// Animate water texture coordinates.
	//

	// Tile water texture.
	XMMATRIX wavesScale = XMMatrixScaling(1.0f, 1.0f, 0.0f);

	// Translate texture over time.
	mWaterTexOffset.y += 0.05f * dt;
	mWaterTexOffset.x += 0.1f * dt;
	XMMATRIX wavesOffset = XMMatrixTranslation(mWaterTexOffset.x, mWaterTexOffset.y, 0.0f);

	// Combine scale and translation.
	XMStoreFloat4x4(&mWaterTexTransform, wavesScale * wavesOffset);
}

vector<D3D11_INPUT_ELEMENT_DESC> TexturedHillsAndWavesDemo::BuildVertexLayout()
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

void TexturedHillsAndWavesDemo::DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer)
{
	graphicsTools.deviceContext->ClearRenderTargetView(graphicsTools.renderTargetView, ColorsFL::LightSteelBlue);
	graphicsTools.deviceContext->ClearDepthStencilView(graphicsTools.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	graphicsTools.deviceContext->IASetInputLayout(graphicsTools.vertexshader.GetInputLayout());
	graphicsTools.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CreateRasterizerState(graphicsTools);

	UINT stride = sizeof(vertexPosNormalTex);
	UINT offset = 0;
	graphicsTools.deviceContext->IASetVertexBuffers(0, 1, mLandVB.GetAddressOf(), &stride, &offset);
	graphicsTools.deviceContext->IASetIndexBuffer(mLandIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	graphicsTools.deviceContext->OMSetDepthStencilState(graphicsTools.depthStencilState, 0);
	graphicsTools.deviceContext->VSSetShader(graphicsTools.vertexshader.GetShader(), NULL, 0);
	graphicsTools.deviceContext->PSSetShader(graphicsTools.pixelshader.GetShader(), NULL, 0);
	graphicsTools.deviceContext->PSSetShaderResources(0, 1, this->mGrassMapSRV.GetAddressOf()); // Texture to shader

	// Draw the model.
	XMMATRIX world = XMLoadFloat4x4(&mLandWorld);

	ConstantBuffer<CH8_ProjectionsTexture> constBuf;
	constBuf.Initialize(graphicsTools.device, graphicsTools.deviceContext);
	constBuf.data.gWorldViewProj = world * graphicsTools.view * graphicsTools.proj;
	constBuf.data.gWorldViewProj = DirectX::XMMatrixTranspose(constBuf.data.gWorldViewProj);
	constBuf.data.gWorld = world;
	constBuf.data.gWorldInvTranspose = MathHelper::InverseTranspose(world);
	constBuf.data.gTexTransform = XMLoadFloat4x4(&mGrassTexTransform);
	if (!constBuf.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constBuf.GetAddressOf());

	ConstantBuffer<CH7_LightPerFrame> cbLightPerFrame;
	cbLightPerFrame.Initialize(graphicsTools.device, graphicsTools.deviceContext);
	cbLightPerFrame.data.gDirLights[0] = mDirLights[0];
	cbLightPerFrame.data.gDirLights[1] = mDirLights[1];
	cbLightPerFrame.data.gDirLights[2] = mDirLights[2];
	cbLightPerFrame.data.gEyePosW = graphicsTools.cameraPosition;
	cbLightPerFrame.data.gMaterial = mLandMat;
	cbLightPerFrame.data.gLightCount = 2;
	if (!cbLightPerFrame.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->PSSetConstantBuffers(1, 1, cbLightPerFrame.GetAddressOf());

	graphicsTools.deviceContext->DrawIndexed(mLandIndexCount, 0, 0);
	
	// Waves

	graphicsTools.deviceContext->IASetVertexBuffers(0, 1, mWavesVB.GetAddressOf(), &stride, &offset);
	graphicsTools.deviceContext->IASetIndexBuffer(mWavesIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	cbLightPerFrame.data.gMaterial = mWavesMat;
	if (!cbLightPerFrame.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->PSSetConstantBuffers(1, 1, cbLightPerFrame.GetAddressOf());

	constBuf.data.gTexTransform = XMLoadFloat4x4(&mWaterTexTransform);
	if (!constBuf.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constBuf.GetAddressOf());

	graphicsTools.deviceContext->PSSetShaderResources(0, 1, this->mWavesMapSRV.GetAddressOf()); // Texture to shader

	graphicsTools.deviceContext->DrawIndexed(3 * mWaves.TriangleCount(), 0, 0);

	graphicsTools.deviceContext->RSSetState(0);
}