#pragma once

#include "AbstractDemo.h"

class LitSkullDemo : public AbstractDemo
{
public:
	bool BuildGeometryBuffers(ID3D11Device* device);
	bool BuildShapesBuffers(ID3D11Device* device);
	void DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer);

	bool Init(ID3D11Device* device);

	vector<D3D11_INPUT_ELEMENT_DESC> BuildVertexLayout();

	GeometryGenerator::MeshData grid;
	GeometryGenerator geoGen;

	VertexBuffer<vertexPosNormal> mShapesVB;
	IndexBuffer mShapesIB;

	VertexBuffer<vertexPosNormal> mSkullVB;
	IndexBuffer mSkullIB;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState; // как растеризуем (заполняем полигоны или нет)

private:
	DirectionalLight mDirLights[3];
	Material mGridMat;
	Material mBoxMat;
	Material mCylinderMat;
	Material mSphereMat;
	Material mSkullMat;

	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mSphereWorld[10];
	XMFLOAT4X4 mCylWorld[10];
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mSkullWorld;

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

	UINT mSkullIndexCount;

	UINT mLightCount;

	XMFLOAT3 mEyePosW;
};
