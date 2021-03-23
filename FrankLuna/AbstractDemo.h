#pragma once

#include <WICTextureLoader.h>

#include "GeometryGenerator.h"
#include "VertexFL.h"
#include "Light.h"
#include "MathHelper.h"

#include "../Graphics/Shaders.h"
#include "../Graphics/VertexBuffer.h"
#include "../Graphics/IndexBuffer.h"
#include "../Graphics/ConstantBuffer.h"


class AbstractDemo
{
public:
	virtual vector<D3D11_INPUT_ELEMENT_DESC> BuildVertexLayout();
	virtual void CreateRasterizerState(GraphicsTools graphicsTools);

protected:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState; // как растеризуем (заполняем полигоны или нет)
};

