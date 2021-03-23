#include "WavesDemo.h"

float WavesDemo::GetHeight(float x, float z) const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

bool WavesDemo::BuildGeometryBuffers(ID3D11Device* device)
{
	if (!BuildLandGeometryBuffers(device))
		return false;

	if (!BuildWavesGeometryBuffers(device))
		return false;

	return true;
}

bool WavesDemo::BuildLandGeometryBuffers(ID3D11Device* device)
{
	GeometryGenerator::MeshData grid;
	GeometryGenerator geoGen;

	geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

	mGridIndexCount = grid.Indices.size();

	//
	// Extract the vertex elements we are interested and apply the height function to
	// each vertex.  In addition, color the vertices based on their height so we have
	// sandy looking beaches, grassy low hills, and snow mountain peaks.
	//

	std::vector<VertexFL> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		p.y = GetHeight(p.x, p.z);

		vertices[i].Pos = p;

		// Color the vertex based on its height.
		if (p.y < -10.0f)
		{
			// Sandy beach color.
			vertices[i].Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if (p.y < 5.0f)
		{
			// Light yellow-green.
			vertices[i].Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if (p.y < 12.0f)
		{
			// Dark yellow-green.
			vertices[i].Color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if (p.y < 20.0f)
		{
			// Dark brown.
			vertices[i].Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else
		{
			// White snow.
			vertices[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
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

bool WavesDemo::BuildWavesGeometryBuffers(ID3D11Device* device)
{
	// Create the vertex buffer.  Note that we allocate space only, as
	// we will be updating the data every time step of the simulation.
	HRESULT hr = this->mWavesVB.Initialize(device, 0, sizeof(VertexFL) * mWaves.VertexCount(), "dynamic");
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

bool WavesDemo::Init(ID3D11Device* device)
{
	timer.Start();

	mRadius = 200.0f;
	mTheta = 1.5f * DirectX::XM_PI;
	mPhi = 0.1f * DirectX::XM_PI;

	mWaves.Init(200, 200, 0.8f, 0.03f, 3.25f, 0.4f);

	if (!BuildLandGeometryBuffers(device))
		return false;

	if (!BuildWavesGeometryBuffers(device))
		return false;

	BuildVertexLayout();

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);
	XMStoreFloat4x4(&mWavesWorld, I);

	return true;
}

void WavesDemo::UpdateScene(GraphicsTools graphicsTools, float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float z = mRadius * sinf(mPhi) * sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	//
	// Every quarter second, generate a random wave.
	//
	static float t_base = 0.0f;
	if ((timer.GetMilisecondsElapsed() - t_base) >= 250.0f)
	{
		t_base += 0.25f;

		DWORD i = 5 + rand() % 190;
		DWORD j = 5 + rand() % 190;

		float r = MathHelper::randMToN(1.0f, 2.0f);
		//float r = MathHelper::RandF(1.0f, 2.0f);

		mWaves.Disturb(i, j, r);
	}

	mWaves.Update(dt);

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

	VertexFL* v = reinterpret_cast<VertexFL*>(mappedData.pData);
	for (UINT i = 0; i < mWaves.VertexCount(); ++i)
	{
		v[i].Pos = mWaves[i];
		v[i].Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	graphicsTools.deviceContext->Unmap(mWavesVB.Get(), 0);
}

void WavesDemo::DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer)
{
	graphicsTools.deviceContext->ClearRenderTargetView(graphicsTools.renderTargetView, ColorsFL::LightSteelBlue);
	graphicsTools.deviceContext->ClearDepthStencilView(graphicsTools.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	graphicsTools.deviceContext->IASetInputLayout(graphicsTools.vertexshader.GetInputLayout());
	graphicsTools.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Create Rasterizer State
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

	HRESULT hr = graphicsTools.device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state.");
		exit(-1);
	}

	graphicsTools.deviceContext->RSSetState(this->rasterizerState.Get());

	UINT stride = sizeof(VertexFL);
	UINT offset = 0;
	graphicsTools.deviceContext->IASetVertexBuffers(0, 1, mLandVB.GetAddressOf(), &stride, &offset);
	graphicsTools.deviceContext->IASetIndexBuffer(mLandIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	graphicsTools.deviceContext->RSSetState(this->rasterizerState.Get());
	graphicsTools.deviceContext->OMSetDepthStencilState(graphicsTools.depthStencilState, 0);
	graphicsTools.deviceContext->VSSetShader(graphicsTools.vertexshader.GetShader(), NULL, 0);
	graphicsTools.deviceContext->PSSetShader(graphicsTools.pixelshader.GetShader(), NULL, 0);

	// Draw the land.
	XMMATRIX world = XMLoadFloat4x4(&mGridWorld);

	constantBuffer.data.mat = world * graphicsTools.view * graphicsTools.proj;
	constantBuffer.data.mat = DirectX::XMMatrixTranspose(constantBuffer.data.mat);
	if (!constantBuffer.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	//graphicsTools.deviceContext->DrawIndexed(mGridIndexCount, 0, 0);

	//
	// Draw the waves.
	//

	//md3dImmediateContext->RSSetState(mWireframeRS);

	graphicsTools.deviceContext->IASetVertexBuffers(0, 1, mWavesVB.GetAddressOf(), &stride, &offset);
	graphicsTools.deviceContext->IASetIndexBuffer(mWavesIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	world = XMLoadFloat4x4(&mWavesWorld);
	graphicsTools.deviceContext->DrawIndexed(3 * mWaves.TriangleCount(), 0, 0);

	// Restore default.
	graphicsTools.deviceContext->RSSetState(0);
}

D3D11_RASTERIZER_DESC* WavesDemo::CreateRasterizedState(GraphicsTools graphicsTools)
{
	//Create Rasterizer State
	D3D11_RASTERIZER_DESC* rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc->FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rasterizerDesc->CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	rasterizerDesc->FrontCounterClockwise = false;
	rasterizerDesc->DepthClipEnable = true;

	return rasterizerDesc;
}