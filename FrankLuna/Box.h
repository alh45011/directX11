#pragma once

#include "AbstractDemo.h"

#include "../Timer.h"

class Box : public AbstractDemo
{
public:
	bool BuildGeometryBuffers(ID3D11Device* device);

	void DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer);
	bool Init(ID3D11Device* device);

private:
	VertexBuffer<VertexFL> vertexBuffer;
	IndexBuffer indicesBuffer;

	Timer timer;
};

