#include "AdapterReader.h"
#include "Shaders.h"
#include "Model.h"
#include "Camera.h"

#include "Triangle.h"

#include <SpriteBatch.h>
#include <SpriteFont.h>

#include <WICTextureLoader.h>

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_win32.h"
#include "ImGUI/imgui_impl_dx11.h"

#include "../FrankLuna/Hills.h"
#include "../FrankLuna/Shapes.h"
#include "../FrankLuna/Skull.h"
#include "../FrankLuna/WavesDemo.h"
#include "../FrankLuna/Box.h"

//Chapter 7
#include "../FrankLuna/LightingDemo.h"
#include "../FrankLuna/LitSkullDemo.h"

//HW
#include "../FrankLuna/CubeExtended_2.h"
#include "../FrankLuna/Pyramid.h"

//Chapter 8
#include "../FrankLuna/CrateDemo.h"
#include "../FrankLuna/TexturedHillsAndWavesDemo.h"

class Graphics
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	void UpdateFrame(float dt);
	void RenderDefaultFrame();

	Camera camera;
	Model model;
private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();

	void SetProjections();

	Microsoft::WRL::ComPtr<ID3D11Device> device; // The ID3D11Device interface is used to check feature support, and allocate resources.
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext; // interface is used to set render states, bind resources to the graphics pipeline, and issue rendering commands
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain; // для двойного буффера
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView; // окно

	VertexShader vertexshader;
	PixelShader pixelshader;
	
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_pixelshader> cb_ps_pixelshader;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_CullFront;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;

	Triangle triangle;

	int windowHeight = 0;
	int windowWidth = 0;	

	Timer fpsTimer;

	//Frank Luna
	Hills hills;
	Shapes shapes;
	Skull skull;
	WavesDemo wavesDemo;
	Box box;

	//Chapter 7
	LightingDemo lightingDemo;
	LitSkullDemo litSkullDemo;

	//HW
	CubeExtended_2 boxExtended;
	Pyramid pyramid;

	//Chapter 8
	CrateDemo crateDemo;
	TexturedHillsAndWavesDemo texturedHillsAndWavesDemo;
};