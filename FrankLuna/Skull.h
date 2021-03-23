#pragma once

#include "AbstractDemo.h"

class Skull : public AbstractDemo
{
public:
	bool BuildGeometryBuffers(ID3D11Device* device);
	void DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer);

	bool Init(ID3D11Device* device);

	void CreateRasterizedState(GraphicsTools graphicsTools);

	GeometryGenerator::MeshData grid;
	GeometryGenerator geoGen;

	VertexBuffer<VertexFL> vertexBuffer;
	IndexBuffer indicesBuffer;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState; // как растеризуем (заполняем полигоны или нет)

private:
	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mSkullWorld;

	UINT mSkullIndexCount;
};

