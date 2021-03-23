#pragma once

#include "AbstractDemo.h"

class Shapes : public AbstractDemo
{	
	public:
		bool Init(ID3D11Device* device);
		bool BuildGeometryBuffers(ID3D11Device* device);
		void DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer);
		
		void CreateRasterizerState(GraphicsTools graphicsTools);

		GeometryGenerator::MeshData grid;
		GeometryGenerator geoGen;

		VertexBuffer<VertexFL> vertexBuffer;
		IndexBuffer indicesBuffer;

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

private:
	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mSphereWorld[10];
	XMFLOAT4X4 mCylWorld[10];
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mCenterSphere;

	int mBoxVertexOffset;
	int mGridVertexOffset;
	int mSphereVertexOffset;
	int mCylinderVertexOffset;

	UINT mBoxIndexOffset;
	UINT mGridIndexOffset;
	UINT mSphereIndexOffset;
	UINT mCylinderIndexOffset;

	UINT mBoxIndexCount;
	UINT mGridIndexCount;
	UINT mSphereIndexCount;
	UINT mCylinderIndexCount;
};