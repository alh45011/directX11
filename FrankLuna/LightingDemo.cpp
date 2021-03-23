#include "LightingDemo.h"

float LightingDemo::GetHillHeight(float x, float z) const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

XMFLOAT3 LightingDemo::GetHillNormal(float x, float z)const
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

bool LightingDemo::BuildLandGeometryBuffers(ID3D11Device* device)
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

	std::vector<vertexPosNormal> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		p.y = GetHillHeight(p.x, p.z);

		vertices[i].Pos = p;
		vertices[i].Normal = GetHillNormal(p.x, p.z);
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

bool LightingDemo::BuildWavesGeometryBuffers(ID3D11Device* device)
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

bool LightingDemo::BuildGeometryBuffers(ID3D11Device* device)
{
	if (!BuildLandGeometryBuffers(device))
		return false;

	if (!BuildWavesGeometryBuffers(device))
		return false;

	return true;
}

bool LightingDemo::Init(ID3D11Device* device)
{
	timer.Start();

	mWaves.Init(160, 160, 1.0f, 0.03f, 3.25f, 0.4f);

	if (!BuildLandGeometryBuffers(device))
		return false;

	if (!BuildWavesGeometryBuffers(device))
		return false;

	BuildVertexLayout();

	mLandWorld = XMMatrixIdentity();
	mWavesWorld = XMMatrixIdentity();
	//XMStoreFloat4x4(&mLandWorld, I);
	//XMStoreFloat4x4(&mWavesWorld, I);

	XMMATRIX wavesOffset = XMMatrixTranslation(0.0f, -3.0f, 0.0f);
	mWavesWorld = wavesOffset;
	//XMStoreFloat4x4(&mWavesWorld, wavesOffset);

	// Directional light.
	mDirLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLight.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	// Point light--position is changed every frame to animate in UpdateScene function.
	mPointLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mPointLight.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mPointLight.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mPointLight.Att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	mPointLight.Range = 25.0f;

	// Spot light--position and direction changed every frame to animate in UpdateScene function.
	mSpotLight.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mSpotLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	mSpotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mSpotLight.Attenuation = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mSpotLight.Spot = 96.0f;
	mSpotLight.Range = 10000.0f;

	mLandMat.Ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mLandMat.Diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mLandMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mWavesMat.Ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	mWavesMat.Diffuse = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);

	mEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);

	return true;
}

vector<D3D11_INPUT_ELEMENT_DESC> LightingDemo::BuildVertexLayout()
{
	// Create the vertex input layout.
	vector<D3D11_INPUT_ELEMENT_DESC> vertexDesc =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	return vertexDesc;
}

void LightingDemo::UpdateScene(GraphicsTools graphicsTools, float dt)
{
	//mEyePosW = XMFLOAT3(x, y, z);
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

	vertexPosNormal* v = reinterpret_cast<vertexPosNormal*>(mappedData.pData);
	for (UINT i = 0; i < mWaves.VertexCount(); ++i)
	{
		v[i].Pos = mWaves[i];
		v[i].Normal = mWaves.Normal(i);
	}

	graphicsTools.deviceContext->Unmap(mWavesVB.Get(), 0);

	//
	// Animate the lights.
	//

	// Circle light over the land surface.
	mPointLight.Position.x = 70.0f * cosf(0.2f * timer.GetMilisecondsElapsed() / 1000.0f);
	mPointLight.Position.z = 70.0f * sinf(0.2f * timer.GetMilisecondsElapsed() / 1000.0f);
	mPointLight.Position.y = max(GetHillHeight(mPointLight.Position.x,
		mPointLight.Position.z), -3.0f) + 10.0f;

	//XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR pos = XMVectorSet(graphicsTools.cameraPosition.x, graphicsTools.cameraPosition.y, graphicsTools.cameraPosition.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// The spotlight takes on the camera position and is aimed in the
	// same direction the camera is looking.  In this way, it looks
	// like we are holding a flashlight.
	mSpotLight.Position = mEyePosW;
	
	//TO DO
	//XMStoreFloat3(&mSpotLight.SpotDirection, XMVector3Normalize(target - pos));
	XMStoreFloat3(&mSpotLight.SpotDirection, graphicsTools.cameraTarget);
	//mSpotLight.SpotDirection = XMStoreFloat3(XMVector3Normalize(target - pos));
}

void LightingDemo::CreateRasterizerState(GraphicsTools graphicsTools)
{
	//Create Rasterizer State
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

	HRESULT hr = graphicsTools.device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state.");
		exit(-1);
	}

	graphicsTools.deviceContext->RSSetState(this->rasterizerState.Get());
}

void LightingDemo::DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer)
{
	graphicsTools.deviceContext->ClearRenderTargetView(graphicsTools.renderTargetView, ColorsFL::LightSteelBlue);
	graphicsTools.deviceContext->ClearDepthStencilView(graphicsTools.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	graphicsTools.deviceContext->IASetInputLayout(graphicsTools.vertexshader.GetInputLayout());
	graphicsTools.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CreateRasterizerState(graphicsTools);

	UINT stride = sizeof(vertexPosNormal);
	UINT offset = 0;
	graphicsTools.deviceContext->IASetVertexBuffers(0, 1, mLandVB.GetAddressOf(), &stride, &offset);
	graphicsTools.deviceContext->IASetIndexBuffer(mLandIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	graphicsTools.deviceContext->RSSetState(this->rasterizerState.Get());
	graphicsTools.deviceContext->OMSetDepthStencilState(graphicsTools.depthStencilState, 0);
	graphicsTools.deviceContext->VSSetShader(graphicsTools.vertexshader.GetShader(), NULL, 0);
	graphicsTools.deviceContext->PSSetShader(graphicsTools.pixelshader.GetShader(), NULL, 0);

	// Draw the land.
	XMMATRIX world = mLandWorld;

	ConstantBuffer<FL_Light_Per_Frame> cbLightPerFrame;
	cbLightPerFrame.Initialize(graphicsTools.device, graphicsTools.deviceContext);
	cbLightPerFrame.data.gDirLight = mDirLight;
	cbLightPerFrame.data.gPointLight = mPointLight;
	cbLightPerFrame.data.gSpotLight = mSpotLight;
	cbLightPerFrame.data.gEyePosW = mEyePosW;

	cbLightPerFrame.ApplyChanges();
	graphicsTools.deviceContext->VSSetConstantBuffers(2, 1, cbLightPerFrame.GetAddressOf());
	graphicsTools.deviceContext->PSSetConstantBuffers(2, 1, cbLightPerFrame.GetAddressOf());

	constantBuffer.data.mat = world * graphicsTools.view * graphicsTools.proj;
	constantBuffer.data.mat = DirectX::XMMatrixTranspose(constantBuffer.data.mat);
	if (!constantBuffer.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	ConstantBuffer<FL_Light_Projections> cbLightProjections;
	cbLightProjections.Initialize(graphicsTools.device, graphicsTools.deviceContext);
	cbLightProjections.data.gWorld = mLandWorld;
	cbLightProjections.data.gWorldInvTranspose = MathHelper::InverseTranspose(world);
	cbLightProjections.data.gMaterial = mLandMat;
	cbLightProjections.ApplyChanges();
	graphicsTools.deviceContext->VSSetConstantBuffers(1, 1, cbLightProjections.GetAddressOf());
	graphicsTools.deviceContext->PSSetConstantBuffers(1, 1, cbLightProjections.GetAddressOf());

	graphicsTools.deviceContext->DrawIndexed(mGridIndexCount, 0, 0);

	//
	// Draw the waves.
	//
	cbLightProjections.data.gWorld = mWavesWorld;
	cbLightProjections.data.gWorldInvTranspose = MathHelper::InverseTranspose(world);
	cbLightProjections.data.gMaterial = mWavesMat;
	cbLightProjections.ApplyChanges();
	graphicsTools.deviceContext->VSSetConstantBuffers(1, 1, cbLightProjections.GetAddressOf());
	graphicsTools.deviceContext->PSSetConstantBuffers(1, 1, cbLightProjections.GetAddressOf());
	//md3dImmediateContext->RSSetState(mWireframeRS);

	graphicsTools.deviceContext->IASetVertexBuffers(0, 1, mWavesVB.GetAddressOf(), &stride, &offset);
	graphicsTools.deviceContext->IASetIndexBuffer(mWavesIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	cbLightPerFrame.ApplyChanges();
	graphicsTools.deviceContext->VSSetConstantBuffers(2, 1, cbLightPerFrame.GetAddressOf());
	graphicsTools.deviceContext->PSSetConstantBuffers(2, 1, cbLightPerFrame.GetAddressOf());

	//world = XMLoadFloat4x4(&mWavesWorld);
	graphicsTools.deviceContext->DrawIndexed(3 * mWaves.TriangleCount(), 0, 0);

	// Restore default.
	graphicsTools.deviceContext->RSSetState(0);
}