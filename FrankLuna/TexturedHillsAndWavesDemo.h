#pragma once

#include "AbstractDemo.h"
#include "../Timer.h"
#include "Waves.h"

class TexturedHillsAndWavesDemo : public AbstractDemo
{
public:
	bool BuildGeometryBuffers(ID3D11Device* device);

	float GetHillHeight(float x, float z)const;
	XMFLOAT3 GetHillNormal(float x, float z)const;
	bool BuildLandGeometryBuffers(ID3D11Device* device);
	bool BuildWavesGeometryBuffers(ID3D11Device* device);

	void UpdateScene(GraphicsTools graphicsTools, float dt);
	void DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer);
	bool Init(ID3D11Device* device);

	vector<D3D11_INPUT_ELEMENT_DESC> BuildVertexLayout();

private:

	VertexBuffer<vertexPosNormalTex> mLandVB;
	IndexBuffer mLandIB;

	VertexBuffer<vertexPosNormalTex> mWavesVB;
	IndexBuffer mWavesIB;

	DirectionalLight mDirLights[3];
	Material mLandMat;
	Material mWavesMat;

	Waves mWaves;	

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState; // как растеризуем (заполняем полигоны или нет)

	Timer timer;

	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mGrassTexTransform;
	XMFLOAT4X4 mWaterTexTransform;
	XMFLOAT4X4 mLandWorld;
	XMFLOAT4X4 mWavesWorld;

	UINT mLandIndexCount;
	XMFLOAT2 mWaterTexOffset;

	XMFLOAT3 mEyePosW;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mGrassMapSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mWavesMapSRV;
};