#pragma once

#include "AbstractDemo.h"

class Hills : public AbstractDemo
{
public:
	bool Init(ID3D11Device* device);
	bool BuildGeometryBuffers(ID3D11Device* device);
	void DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer);

	float GetHeight(float x, float z) const;

private:
	GeometryGenerator::MeshData grid;
	GeometryGenerator geoGen;

	VertexBuffer<VertexFL> vertexBuffer;
	IndexBuffer indicesBuffer;
};

