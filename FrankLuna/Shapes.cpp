#include "Shapes.h"

#include "../ErrorLogger.h"

bool Shapes::Init(ID3D11Device* device)
{
	if (!BuildGeometryBuffers(device))
		return false;

	BuildVertexLayout();

	return true;
}

bool Shapes::BuildGeometryBuffers(ID3D11Device* device)
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 60, 40, grid);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	geoGen.CreateGeosphere(0.5f, 2, sphere);
	geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset = 0;
	mGridVertexOffset = box.Vertices.size();
	mSphereVertexOffset = mGridVertexOffset + grid.Vertices.size();
	mCylinderVertexOffset = mSphereVertexOffset + sphere.Vertices.size();

	// Cache the index count of each object.
	mBoxIndexCount = box.Indices.size();
	mGridIndexCount = grid.Indices.size();
	mSphereIndexCount = sphere.Indices.size();
	mCylinderIndexCount = cylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset = 0;
	mGridIndexOffset = mBoxIndexCount;
	mSphereIndexOffset = mGridIndexOffset + mGridIndexCount;
	mCylinderIndexOffset = mSphereIndexOffset + mSphereIndexCount;

	UINT totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	UINT totalIndexCount =
		mBoxIndexCount +
		mGridIndexCount +
		mSphereIndexCount +
		mCylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<VertexFL> vertices(totalVertexCount);

	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Color = black;
	}

	HRESULT hr = this->vertexBuffer.Initialize(device, vertices.data(), vertices.size());
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
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	hr = this->indicesBuffer.Initialize(device, indices.data(), indices.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return hr;
	}

	return true;
}

void Shapes::CreateRasterizerState(GraphicsTools graphicsTools)
{
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
}

void Shapes::DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer)
{	
	graphicsTools.deviceContext->ClearRenderTargetView(graphicsTools.renderTargetView, ColorsFL::LightSteelBlue);
	graphicsTools.deviceContext->ClearDepthStencilView(graphicsTools.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	graphicsTools.deviceContext->IASetInputLayout(graphicsTools.vertexshader.GetInputLayout());
	graphicsTools.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CreateRasterizerState(graphicsTools);

	UINT stride = sizeof(VertexFL);
	UINT offset = 0;
	graphicsTools.deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	graphicsTools.deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	
	graphicsTools.deviceContext->OMSetDepthStencilState(graphicsTools.depthStencilState, 0);
	graphicsTools.deviceContext->VSSetShader(graphicsTools.vertexshader.GetShader(), NULL, 0);
	graphicsTools.deviceContext->PSSetShader(graphicsTools.pixelshader.GetShader(), NULL, 0);

	// Draw the grid.
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);
	XMMATRIX world = XMLoadFloat4x4(&mGridWorld);
	graphicsTools.deviceContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

	// Draw the box.
	XMMATRIX boxScale = XMMatrixScaling(2.0f, 1.0f, 2.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));
	world = XMLoadFloat4x4(&mBoxWorld);

	constantBuffer.data.mat = world * graphicsTools.view * graphicsTools.proj;
	constantBuffer.data.mat = DirectX::XMMatrixTranspose(constantBuffer.data.mat);
		if (!constantBuffer.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	graphicsTools.deviceContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

	// Draw center sphere.
	XMMATRIX centerSphereScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
	XMMATRIX centerSphereOffset = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	XMStoreFloat4x4(&mCenterSphere, XMMatrixMultiply(centerSphereScale, centerSphereOffset));
	world = XMLoadFloat4x4(&mCenterSphere);
	
	constantBuffer.data.mat = world * graphicsTools.view * graphicsTools.proj;
	constantBuffer.data.mat = DirectX::XMMatrixTranspose(constantBuffer.data.mat);
	if (!constantBuffer.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	graphicsTools.deviceContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);

	// Draw the cylinders.
	for (int i = 0; i < 5; ++i)
	{
		XMStoreFloat4x4(&mCylWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f));
		XMStoreFloat4x4(&mCylWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f));

		XMStoreFloat4x4(&mSphereWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f));
		XMStoreFloat4x4(&mSphereWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f));
	}

	for (int i = 0; i < 10; ++i)
	{
		world = XMLoadFloat4x4(&mCylWorld[i]);
		constantBuffer.data.mat = world * graphicsTools.view * graphicsTools.proj;
		constantBuffer.data.mat = DirectX::XMMatrixTranspose(constantBuffer.data.mat);
		if (!constantBuffer.ApplyChanges())
			exit(-1);
		graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

		graphicsTools.deviceContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
	}

	// Draw the spheres.
	for (int i = 0; i < 10; ++i)
	{
		world = XMLoadFloat4x4(&mSphereWorld[i]);
		constantBuffer.data.mat = world * graphicsTools.view * graphicsTools.proj;
		constantBuffer.data.mat = DirectX::XMMatrixTranspose(constantBuffer.data.mat);
		if (!constantBuffer.ApplyChanges())
			exit(-1);
		graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

		graphicsTools.deviceContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
	}
}
