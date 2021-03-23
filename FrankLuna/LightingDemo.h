#pragma once

#include "AbstractDemo.h"
#include "Waves.h"
#include "../Timer.h"

class LightingDemo : public AbstractDemo
{
public:
	bool BuildGeometryBuffers(ID3D11Device* device);
	bool BuildLandGeometryBuffers(ID3D11Device* device);
	bool BuildWavesGeometryBuffers(ID3D11Device* device);

	void DrawScene(GraphicsTools graphicsTools, ConstantBuffer<CB_VS_vertexshader>& constantBuffer);
	void UpdateScene(GraphicsTools graphicsTools, float dt);
	bool Init(ID3D11Device* device);

	void CreateRasterizerState(GraphicsTools graphicsTools);
	vector<D3D11_INPUT_ELEMENT_DESC> BuildVertexLayout();

private:
	float GetHillHeight(float x, float z) const;
	XMFLOAT3 GetHillNormal(float x, float z)const;

	GeometryGenerator::MeshData grid;
	GeometryGenerator geoGen;

	VertexBuffer<vertexPosNormal> mLandVB;
	IndexBuffer mLandIB;
	VertexBuffer<vertexPosNormal> mWavesVB;
	IndexBuffer mWavesIB;

	Waves mWaves;

	DirectionalLight mDirLight;
	PointLight mPointLight;
	SpotLight mSpotLight;

	Material mLandMat;
	Material mWavesMat;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState; // как растеризуем (заполняем полигоны или нет)

	UINT mGridIndexCount;

	Timer timer;

	// Define transformations from local spaces to world space.
	XMMATRIX mLandWorld;
	XMMATRIX mWavesWorld;

	XMFLOAT3 mEyePosW;
};

