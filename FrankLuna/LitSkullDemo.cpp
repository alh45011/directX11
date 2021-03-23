#include "LitSkullDemo.h"

#include "fstream"

bool LitSkullDemo::Init(ID3D11Device* device)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);

	XMMATRIX boxScale = XMMatrixScaling(3.0f, 1.0f, 3.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX skullScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX skullOffset = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	XMStoreFloat4x4(&mSkullWorld, XMMatrixMultiply(skullScale, skullOffset));
	
	for (int i = 0; i < 5; ++i)
	{
		XMStoreFloat4x4(&mCylWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f));
		XMStoreFloat4x4(&mCylWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f));

		XMStoreFloat4x4(&mSphereWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f));
		XMStoreFloat4x4(&mSphereWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f));
	}

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

	mGridMat.Ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mGridMat.Diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mGridMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mCylinderMat.Ambient = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
	mCylinderMat.Diffuse = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
	mCylinderMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

	mSphereMat.Ambient = XMFLOAT4(0.1f, 0.2f, 0.3f, 1.0f);
	mSphereMat.Diffuse = XMFLOAT4(0.2f, 0.4f, 0.6f, 1.0f);
	mSphereMat.Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);

	mBoxMat.Ambient = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
	mBoxMat.Diffuse = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mSkullMat.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mSkullMat.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mSkullMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

	if (!BuildGeometryBuffers(device))
		return false;

	if (!BuildShapesBuffers(device))
		return false;

	BuildVertexLayout();

	mLightCount = 1;

	return true;
}

bool LitSkullDemo::BuildShapesBuffers(ID3D11Device* device)
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 60, 40, grid);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
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

	std::vector<vertexPosNormal> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
	}

	HRESULT hr = this->mShapesVB.Initialize(device, vertices.data(), vertices.size());
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

	hr = this->mShapesIB.Initialize(device, indices.data(), indices.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return hr;
	}

	return true;
}

bool LitSkullDemo::BuildGeometryBuffers(ID3D11Device* device)
{
	std::ifstream fin("Models/skull.txt");

	if (!fin)
	{
		MessageBox(0, L"Models/skull.txt not found.", 0, 0);
		return false;
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<vertexPosNormal> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	mSkullIndexCount = 3 * tcount;
	std::vector<UINT> indices(mSkullIndexCount);
	for (UINT i = 0; i < tcount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();

	HRESULT hr = this->mSkullVB.Initialize(device, vertices.data(), vertices.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	hr = this->mSkullIB.Initialize(device, indices.data(), indices.size());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return hr;
	}

	return true;
}

vector<D3D11_INPUT_ELEMENT_DESC> LitSkullDemo::BuildVertexLayout()
{
	// Create the vertex input layout.
	vector<D3D11_INPUT_ELEMENT_DESC> vertexDesc =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	return vertexDesc;
}

void LitSkullDemo::DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer)
{
	graphicsTools.deviceContext->ClearRenderTargetView(graphicsTools.renderTargetView, ColorsFL::LightSteelBlue);
	graphicsTools.deviceContext->ClearDepthStencilView(graphicsTools.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	graphicsTools.deviceContext->IASetInputLayout(graphicsTools.vertexshader.GetInputLayout());
	graphicsTools.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CreateRasterizerState(graphicsTools);

	UINT stride = sizeof(vertexPosNormal);
	UINT offset = 0;
	graphicsTools.deviceContext->IASetVertexBuffers(0, 1, mShapesVB.GetAddressOf(), &stride, &offset);
	graphicsTools.deviceContext->IASetIndexBuffer(mShapesIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	graphicsTools.deviceContext->OMSetDepthStencilState(graphicsTools.depthStencilState, 0);
	graphicsTools.deviceContext->VSSetShader(graphicsTools.vertexshader.GetShader(), NULL, 0);
	graphicsTools.deviceContext->PSSetShader(graphicsTools.pixelshader.GetShader(), NULL, 0);

	// Draw the model.
	XMMATRIX world = XMLoadFloat4x4(&mBoxWorld);	

	ConstantBuffer<CH7_Projections> constBuf;
	constBuf.Initialize(graphicsTools.device, graphicsTools.deviceContext);
	constBuf.data.gWorldViewProj = world * graphicsTools.view * graphicsTools.proj;
	constBuf.data.gWorldViewProj = DirectX::XMMatrixTranspose(constBuf.data.gWorldViewProj);
	constBuf.data.gWorld = world;
	constBuf.data.gWorldInvTranspose = MathHelper::InverseTranspose(world);
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

	graphicsTools.deviceContext->DrawIndexed(mBoxIndexCount, 0, 0);

	world = XMLoadFloat4x4(&mGridWorld);	

	//constBuf.Initialize(graphicsTools.device, graphicsTools.deviceContext);
	constBuf.data.gWorldViewProj = world * graphicsTools.view * graphicsTools.proj;
	constBuf.data.gWorldViewProj = DirectX::XMMatrixTranspose(constBuf.data.gWorldViewProj);
	constBuf.data.gWorld = world;
	constBuf.data.gWorldInvTranspose = MathHelper::InverseTranspose(world);
	if (!constBuf.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constBuf.GetAddressOf());

	cbLightPerFrame.data.gMaterial = mGridMat;
	if (!cbLightPerFrame.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->PSSetConstantBuffers(1, 1, cbLightPerFrame.GetAddressOf());

	graphicsTools.deviceContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

	// Draw the cylinders.
	for (int i = 0; i < 10; ++i)
	{
		world = XMLoadFloat4x4(&mCylWorld[i]);
		constBuf.data.gWorldViewProj = world * graphicsTools.view * graphicsTools.proj;
		constBuf.data.gWorldViewProj = DirectX::XMMatrixTranspose(constBuf.data.gWorldViewProj);
		constBuf.data.gWorld = world;
		constBuf.data.gWorldInvTranspose = MathHelper::InverseTranspose(world);
		if (!constBuf.ApplyChanges())
			exit(-1);
		graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constBuf.GetAddressOf());

		cbLightPerFrame.data.gMaterial = mCylinderMat;
		if (!cbLightPerFrame.ApplyChanges())
			exit(-1);
		graphicsTools.deviceContext->PSSetConstantBuffers(1, 1, cbLightPerFrame.GetAddressOf());

		graphicsTools.deviceContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
	}

	// Draw the spheres.
	for (int i = 0; i < 10; ++i)
	{
		world = XMLoadFloat4x4(&mSphereWorld[i]);
		constBuf.data.gWorldViewProj = world * graphicsTools.view * graphicsTools.proj;
		constBuf.data.gWorldViewProj = DirectX::XMMatrixTranspose(constBuf.data.gWorldViewProj);
		constBuf.data.gWorld = world;
		constBuf.data.gWorldInvTranspose = MathHelper::InverseTranspose(world);
		if (!constBuf.ApplyChanges())
			exit(-1);
		graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constBuf.GetAddressOf());

		cbLightPerFrame.data.gMaterial = mSphereMat;
		if (!cbLightPerFrame.ApplyChanges())
			exit(-1);
		graphicsTools.deviceContext->PSSetConstantBuffers(1, 1, cbLightPerFrame.GetAddressOf());

		graphicsTools.deviceContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
	}

	// Draw the skull.
	graphicsTools.deviceContext->IASetVertexBuffers(0, 1, mSkullVB.GetAddressOf(), &stride, &offset);
	graphicsTools.deviceContext->IASetIndexBuffer(mSkullIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	world = XMLoadFloat4x4(&mSkullWorld);
	constBuf.data.gWorldViewProj = world * graphicsTools.view * graphicsTools.proj;
	constBuf.data.gWorldViewProj = DirectX::XMMatrixTranspose(constBuf.data.gWorldViewProj);
	constBuf.data.gWorld = world;
	constBuf.data.gWorldInvTranspose = MathHelper::InverseTranspose(world);
	if (!constBuf.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->VSSetConstantBuffers(0, 1, constBuf.GetAddressOf());

	cbLightPerFrame.data.gMaterial = mSkullMat;
	if (!cbLightPerFrame.ApplyChanges())
		exit(-1);
	graphicsTools.deviceContext->PSSetConstantBuffers(1, 1, cbLightPerFrame.GetAddressOf());

	graphicsTools.deviceContext->DrawIndexed(mSkullIndexCount, 0, 0);
}