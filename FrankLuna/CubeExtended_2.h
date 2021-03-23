#pragma once

#include "AbstractDemo.h"

struct Cube02FL_Pos
{
	XMFLOAT3 Pos;
};

struct Cube02FL_Color
{
	XMFLOAT4 Color;
};

class CubeExtended_2 : public AbstractDemo
{
public:
	bool BuildGeometryBuffers(ID3D11Device* device);
	virtual vector<D3D11_INPUT_ELEMENT_DESC> BuildVertexLayout();

	void DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer);
	bool Init(ID3D11Device* device);

private:
	VertexBuffer<Cube02FL_Pos> vertexBufferOne;
	VertexBuffer<Cube02FL_Color> vertexBufferTwo;
	IndexBuffer indicesBuffer;

};

