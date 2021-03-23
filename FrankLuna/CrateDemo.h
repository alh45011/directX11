#pragma once

#include "AbstractDemo.h"
#include "../Timer.h"

class CrateDemo : public AbstractDemo
{
public:
	bool BuildGeometryBuffers(ID3D11Device* device);

	void DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer);
	bool Init(ID3D11Device* device);

	vector<D3D11_INPUT_ELEMENT_DESC> BuildVertexLayout();

private:

	VertexBuffer<vertexPosNormalTex> mBoxVB;
	IndexBuffer mBoxIB;

	DirectionalLight mDirLights[3];
	Material mBoxMat;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState; // как растеризуем (заполняем полигоны или нет)

	Timer timer;

	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mTexTransform;
	XMFLOAT4X4 mBoxWorld;

	int mBoxVertexOffset;
	UINT mBoxIndexOffset;
	UINT mBoxIndexCount;

	XMFLOAT3 mEyePosW;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myTexture;
};

