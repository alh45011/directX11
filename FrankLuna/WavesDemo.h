#pragma once

#include "AbstractDemo.h"
#include "Waves.h"
#include "../Timer.h"

class WavesDemo : public AbstractDemo
{
public:
	bool BuildGeometryBuffers(ID3D11Device* device);
	bool BuildLandGeometryBuffers(ID3D11Device* device);
	bool BuildWavesGeometryBuffers(ID3D11Device* device);

	void DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer);
	void UpdateScene(GraphicsTools graphicsTools, float dt);
	bool Init(ID3D11Device* device);

	D3D11_RASTERIZER_DESC* CreateRasterizedState(GraphicsTools graphicsTools);	

private:
	float GetHeight(float x, float z) const;

	GeometryGenerator::MeshData grid;
	GeometryGenerator geoGen;

	VertexBuffer<VertexFL> mLandVB;
	IndexBuffer mLandIB;
	VertexBuffer<VertexFL> mWavesVB;
	IndexBuffer mWavesIB;

	Waves mWaves;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState; // как растеризуем (заполняем полигоны или нет)

	UINT mGridIndexCount;

	float mTheta;
	float mPhi;
	float mRadius;

	Timer timer;

	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mWavesWorld;
};