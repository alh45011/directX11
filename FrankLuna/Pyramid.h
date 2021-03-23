#pragma once
#pragma once

#include "AbstractDemo.h"

class Pyramid : public AbstractDemo
{
public:
	bool BuildGeometryBuffers(ID3D11Device* device);

	void DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer);
	bool Init(ID3D11Device* device);

	D3D11_RASTERIZER_DESC* CreateRasterizedState(GraphicsTools graphicsTools);

private:
	VertexBuffer<VertexFL> vertexBuffer;
	IndexBuffer indicesBuffer;

};



