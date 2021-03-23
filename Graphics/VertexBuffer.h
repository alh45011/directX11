#ifndef VertexBuffer_h__
#define VertexBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>

#include <memory>
#include <string>

/*
	In order for the GPU to access an array of vertices, they need to be placed in a special resource structure called a buffer, which is
	represented by the ID3D11Buffer interface
*/

template<class T>
class VertexBuffer
{
private:
	VertexBuffer(const VertexBuffer<T>& rhs);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	std::unique_ptr<UINT> stride;
	UINT bufferSize = 0;	

public:
	VertexBuffer() 
	{		
	}

	D3D11_BUFFER_DESC SetBufferSettings(std::string type, UINT numVertices)
	{
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		if (type == "default")
		{			
			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;
		}
		else if (type == "dynamic")
		{
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vertexBufferDesc.MiscFlags = 0;
		}
		
		return vertexBufferDesc;
	}

	ID3D11Buffer* Get()const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	UINT BufferSize() const
	{
		return this->bufferSize;
	}

	const UINT Stride() const
	{
		return *this->stride.get();
	}

	const UINT* StridePtr() const
	{
		return this->stride.get();
	}
	
	HRESULT Initialize(ID3D11Device* device, T* data, UINT numVertices, std::string type = "default")
	{
		if (buffer.Get() != nullptr)
			buffer.Reset();

		this->bufferSize = numVertices;

		if (this->stride.get() == nullptr)
			this->stride = std::make_unique<UINT>(sizeof(T));

		D3D11_BUFFER_DESC vertexBufferDesc = SetBufferSettings(type, numVertices);

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		HRESULT hr;
		if (type == "default")
		{
			hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->buffer.GetAddressOf());
		}			
		
		if (type == "dynamic")
		{
			hr = device->CreateBuffer(&vertexBufferDesc, 0, this->buffer.GetAddressOf());
		}
			
		return hr;
	}
};

#endif // VertexBuffer_h__