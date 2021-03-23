#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	this->fpsTimer.Start();

	if (!InitializeDirectX(hwnd))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;

	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();

	if (!triangle.Initialize(this->device.Get()))
		return false;

	return true;
}

void Graphics::RenderDefaultFrame()
{
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//ID3D11RasterizerState: This interface represents a state group used to configure the rasterization stage of the pipeline.
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);

	UINT offset = 0;

	// Update constant buffer
	//DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
	//constantBuffer.data.mat = world;		

	{
		this->model.Draw(camera.GetViewMatrix() * camera.GetProjectionMatrix());
	}

	//static float alpha = 1.0f;
	//{ // pink
	//	//Update Constant Buffer
	//	static float translationOffset[3] = { 0, 0, -1.0 };
	//	XMMATRIX world = XMMatrixTranslation(translationOffset[0], translationOffset[1], translationOffset[2]);
	//	cv_vs_vertexshader.data.mat = world * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	//	cv_vs_vertexshader.data.mat = DirectX::XMMatrixTranspose(cv_vs_vertexshader.data.mat);

	//	if (!cv_vs_vertexshader.ApplyChanges())
	//		return;
	//	this->deviceContext->VSSetConstantBuffers(0, 1, this->cv_vs_vertexshader.GetAddressOf());

	//	this->cb_ps_pixelshader.data.alpha = alpha;
	//	this->cb_ps_pixelshader.ApplyChanges();
	//	this->deviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_pixelshader.GetAddressOf());

	//	//Red Tri
	//	this->deviceContext->PSSetShaderResources(0, 1, this->pavementTexture.GetAddressOf()); // Texture to shader
	//	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	//	this->deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//	this->deviceContext->DrawIndexed(indicesBuffer.BufferSize(), 0, 0);
	//}

	//{ // pink
	//	//Update Constant Buffer
	//	static float translationOffset[3] = { 0, 0, 5.0 };
	//	XMMATRIX world = XMMatrixTranslation(translationOffset[0], translationOffset[1], translationOffset[2]);
	//	cb_vs_vertexshader.data.mat = world * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	//	cv_vs_vertexshader.data.mat = DirectX::XMMatrixTranspose(cv_vs_vertexshader.data.mat);

	//	if (!cv_vs_vertexshader.ApplyChanges())
	//		return;
	//	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader.GetAddressOf());

	//	this->cb_ps_pixelshader.data.alpha = 1.0;
	//	this->cb_ps_pixelshader.ApplyChanges();
	//	this->deviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_pixelshader.GetAddressOf());

	//	//Red Tri
	//	this->deviceContext->PSSetShaderResources(0, 1, this->grassTexture.GetAddressOf()); // Texture to shader
	//	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	//	this->deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//	this->deviceContext->RSSetState(this->rasterizerState_CullFront.Get());
	//	this->deviceContext->DrawIndexed(indicesBuffer.BufferSize(), 0, 0);

	//	this->deviceContext->RSSetState(this->rasterizerState.Get());
	//	this->deviceContext->DrawIndexed(indicesBuffer.BufferSize(), 0, 0);
	//}

	//Draw Text
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), L"HELLO WORLD", DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	static int counter = 0;
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//Create ImGui Test Window
	ImGui::Begin("Test");
	//Create ImGui Test Window
	ImGui::Text("This is example text.");
	if (ImGui::Button("CLICK ME!"))
		counter += 1;
	ImGui::SameLine();
	std::string clickCount = "Click Count: " + std::to_string(counter);
	ImGui::Text(clickCount.c_str());
	//ImGui::DragFloat("Alpha", &alpha, 0.1f, 0.0f, 1.0f);
	ImGui::End();
	//Assemble Together Draw Data
	ImGui::Render();
	//Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	this->swapchain->Present(1, NULL);
}

void Graphics::SetProjections()
{
	// Update constant buffer
	DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
	cb_vs_vertexshader.data.mat = world;
	
	cb_vs_vertexshader.data.mat = world * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	cb_vs_vertexshader.data.mat = DirectX::XMMatrixTranspose(cb_vs_vertexshader.data.mat);

	if (!cb_vs_vertexshader.ApplyChanges())
		return;
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader.GetAddressOf());
}

void Graphics::UpdateFrame(float dt)
{
	GraphicsTools gt;
	gt.device = device.Get();
	gt.deviceContext = deviceContext.Get();
	gt.renderTargetView = this->renderTargetView.Get();
	gt.depthStencilView = this->depthStencilView.Get();
	gt.depthStencilState = this->depthStencilState.Get();
	gt.vertexshader = this->vertexshader;
	gt.pixelshader = this->pixelshader;
	gt.view = camera.GetViewMatrix();
	gt.proj = camera.GetProjectionMatrix();
	gt.cameraPosition = camera.GetPositionFloat3();
	gt.cameraTarget = camera.GetCamTargetVector();

	//wavesDemo.UpdateScene(gt, dt);
	//lightingDemo.UpdateScene(gt, dt);
	texturedHillsAndWavesDemo.UpdateScene(gt, dt);
}

void Graphics::RenderFrame()
{
	SetProjections();

	GraphicsTools gt;
	gt.device = device.Get();
	gt.deviceContext = deviceContext.Get();
	gt.renderTargetView = this->renderTargetView.Get();
	gt.depthStencilView = this->depthStencilView.Get();
	gt.depthStencilState = this->depthStencilState.Get();
	gt.vertexshader = this->vertexshader;
	gt.pixelshader = this->pixelshader;
	gt.view = camera.GetViewMatrix();
	gt.proj = camera.GetProjectionMatrix();
	gt.cameraPosition = camera.GetPositionFloat3();
	gt.cameraTarget = camera.GetCamTargetVector();

	//box.DrawScene(gt, constantBuffer);
	//hills.DrawScene(gt, constantBuffer);
	//shapes.DrawScene(gt, constantBuffer);
	//skull.DrawScene(gt, constantBuffer);
	/*wavesDemo.UpdateScene(gt, 0.5f);*/
	//wavesDemo.DrawScene(gt, constantBuffer);		
	//boxExtended.DrawScene(gt, constantBuffer);
	//pyramid.DrawScene(gt, constantBuffer);
	
	//CHAPTER 7
	//lightingDemo.DrawScene(gt, constantBuffer);
	//litSkullDemo.DrawScene(gt, constantBuffer);

	//CHAPTER 8
	//crateDemo.DrawScene(gt, constantBuffer);
	texturedHillsAndWavesDemo.DrawScene(gt, cb_vs_vertexshader);

	//Draw Text
	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}

	//Draw Text
	spriteBatch->Begin();	
	spriteFont->DrawString(spriteBatch.get(), StringConverter::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	static int counter = 0;
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//Create ImGui Test Window
	ImGui::Begin("Test");
	//Create ImGui Test Window
	ImGui::Text("This is example text.");
	if (ImGui::Button("CLICK ME!"))
		counter += 1;
	ImGui::SameLine();
	std::string clickCount = "Click Count: " + std::to_string(counter);
	ImGui::Text(clickCount.c_str());
	//ImGui::DragFloat3("Translation X/Y/Z", translationOffset, 0.1f, -5.0f, 5.0f);
	ImGui::End();
	//Assemble Together Draw Data
	ImGui::Render();
	//Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	this->swapchain->Present(1, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	try
	{
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.size() < 1)
		{
			ErrorLogger::Log("No IDXGI Adapters found.");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC scd;
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
		
		scd.BufferDesc.Width = this->windowWidth;
		scd.BufferDesc.Height = this->windowHeight;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;

		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = hwnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT hr;
		hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, //IDXGI Adapter
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL, //FOR SOFTWARE DRIVER TYPE
			NULL, //FLAGS FOR RUNTIME LAYERS
			NULL, //FEATURE LEVELS ARRAY
			0, //# OF FEATURE LEVELS IN ARRAY	
			D3D11_SDK_VERSION,
			&scd, //Swapchain description
			this->swapchain.GetAddressOf(), //Swapchain Address
			this->device.GetAddressOf(), //Device Address
			NULL, //Supported feature level
			this->deviceContext.GetAddressOf()); //Device Context Address

		COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");
		
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");

		// Привязываем его к output merger
		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");
		
		//Describe our Depth/Stencil Buffer. Тут, кроме прочего, описывается формат буфера (текстуры)
		CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
		depthStencilTextureDesc.MipLevels = 1;
		depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		// выделяем память для буфера глубины через device
		hr = this->device->CreateTexture2D(&depthStencilTextureDesc, NULL, this->depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

		// создаём этот буфер (на видеокарте?)
		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

		// биндим targetView при помощи deviceContext для back targetView & depth targetView
		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		//Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		//Create & set the Viewport - область окна(экрана), в которую мы рисуем.
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));;
		this->deviceContext->RSSetViewports(1, &viewport);

		//Create Rasterizer State
		//CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		//rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//Create Rasterizer State for culling
		D3D11_RASTERIZER_DESC rasterizerDesc_CullFront;
		ZeroMemory(&rasterizerDesc_CullFront, sizeof(D3D11_RASTERIZER_DESC));

		rasterizerDesc_CullFront.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		//rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState_CullFront.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//Create Blend State
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));

		D3D11_RENDER_TARGET_BLEND_DESC rtbd;
		ZeroMemory(&rtbd, sizeof(rtbd));

		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[0] = rtbd;

		hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

		//Create sampler description for sampler state
		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf()); //Create sampler state
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}

bool Graphics::InitializeShaders()
{

	std::wstring shaderfolder = L"";
#pragma region DetermineShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG //Debug Mode
#ifdef _WIN64 //x64
		shaderfolder = L"..\\x64\\Debug\\";
#else  //x86 (Win32)
		shaderfolder = L"..\\Debug\\";
#endif
#else //Release Mode
#ifdef _WIN64 //x64
		shaderfolder = L"..\\x64\\Release\\";
#else  //x86 (Win32)
		shaderfolder = L"..\\Release\\";
#endif
#endif
	}

	// параметры vertex
	/*
	1. SemanticName: A string to associate with the element. This can be any valid variable name. Semantics are used to map elements
		in the vertex structure to elements in the vertex shader input signature;
	2. An index to attach to a semantic. The motivation for this is also illustrated in Figure 6.1, where a vertex structure
		may have more than one set of texture coordinates; so rather than introducing a new semantic name, we can attach an index to the
		end to distinguish the texture coordinates. A semantic with no index specified in the shader code defaults to index zero; 
		TEXCOORD0
		TEXCOORD1
	3. Format: A member of the DXGI_FORMAT enumerated type specifying the format (i.e., the data type) of this vertex element to
		Direct3D; here are some common examples of formats used:
	4. InputSlot: Specifies the input slot index this element will come from. Direct3D supports sixteen input slots (indexed from 0−15)
		through which you can feed vertex data. For instance, if a vertex consisted of position and color elements, you could either feed
		both elements through a single input slot, or you could split the elements up and feed the position elements through the first input
		slot and feed the color elements through the second input slot. Direct3D will then use the elements from the different input slots to
		assemble the vertices. In this book, we only use one input slot, but Exercise 2 asks you to experiment with two.
	5. AlignedByteOffset: For a single input slot, this is the offset, in bytes, from the start of the C++ vertex structure to the start of the
		vertex component. For example, in the following vertex structure, the element Pos has a 0-byte offset since its start coincides with
		the start of the vertex structure; the element Normal has a 12-byte offset because we have to skip over the bytes of Pos to get to
		the start of Normal; the element Tex0 has a 24-byte offset because we need to skip over the bytes of Pos and Normal to get to the
		start of Tex0; the element Tex1 has a 32-byte offset because we need to skip over the bytes of Pos, Normal, and Tex0 to get to
		the start of Tex1. (сдвиг) Можно использовать макрос для автоматизации.
	6. InputSlotClass: Specify D3D11_INPUT_PER_VERTEX_DATA for now; the other option is used for the advanced technique of
		instancing.
	7. InstanceDataStepRate: Specify 0 for now; other values are only used for the advanced technique of instancing.
	*/

	//ORIGINAL
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	UINT numElements = ARRAYSIZE(layout);

	if (!vertexshader.Initialize(this->device, shaderfolder + L"vertexshader.cso", layout, numElements))
		return false;

	if (!pixelshader.Initialize(this->device, shaderfolder + L"pixelshader.cso"))
		return false;	

	//Frank Luna
	//vector<D3D11_INPUT_ELEMENT_DESC> layout = hills.BuildVertexLayout();

	// CHAPTER6
	/*vector<D3D11_INPUT_ELEMENT_DESC> layout = shapes.BuildVertexLayout();
	UINT numElements = layout.size();	
	
	if (!vertexshader.Initialize(this->device, shaderfolder + L"vertexshaderFL.cso", layout.data(), numElements))
		return false;

	if (!pixelshader.Initialize(this->device, shaderfolder + L"pixelshaderFL.cso"))
		return false;*/

	// CHAPTER7
	//Lighting Demo
	/*vector<D3D11_INPUT_ELEMENT_DESC> layout = lightingDemo.BuildVertexLayout();
	UINT numElements = layout.size();*/

	/*if (!vertexshader.Initialize(this->device, shaderfolder + L"FLvertexShaderLight.cso", layout.data(), numElements))
		return false;

	if (!pixelshader.Initialize(this->device, shaderfolder + L"FLpixelShaderLight.cso"))
		return false;*/

	// LitSkullDemo
	//vector<D3D11_INPUT_ELEMENT_DESC> layout = litSkullDemo.BuildVertexLayout();
	//UINT numElements = layout.size();

	//if (!vertexshader.Initialize(this->device, shaderfolder + L"litSkullVS.cso", layout.data(), numElements))
	//	return false;

	//if (!pixelshader.Initialize(this->device, shaderfolder + L"litSkullPS.cso"))
	//	return false;

	// CrateDemo
	//vector<D3D11_INPUT_ELEMENT_DESC> layout = crateDemo.BuildVertexLayout();
	//UINT numElements = layout.size();

	//if (!vertexshader.Initialize(this->device, shaderfolder + L"cubeDemoVS.cso", layout.data(), numElements))
	//	return false;

	//if (!pixelshader.Initialize(this->device, shaderfolder + L"cubeDemoPS.cso"))
	//	return false;

	// CrateDemo
	/*vector<D3D11_INPUT_ELEMENT_DESC> layout = texturedHillsAndWavesDemo.BuildVertexLayout();
	UINT numElements = layout.size();

	if (!vertexshader.Initialize(this->device, shaderfolder + L"cubeDemoVS.cso", layout.data(), numElements))
		return false;

	if (!pixelshader.Initialize(this->device, shaderfolder + L"cubeDemoPS.cso"))
		return false;*/

	return true;
}

bool Graphics::InitializeScene()
{
	try
	{		
		HRESULT hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\seamless_grass.jpg", nullptr, grassTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\pinksquare.jpg", nullptr, pinkTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\seamless_pavement.jpg", nullptr, pavementTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		//Initialize Constant Buffer(s)
		hr = this->cb_vs_vertexshader.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		hr = this->cb_ps_pixelshader.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

		camera.SetPosition(0.0f, 0.0f, -2.0f);
		camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);

		if (!model.Initialize(this->device.Get(), this->deviceContext.Get(), this->pavementTexture.Get(), this->cb_vs_vertexshader))
			return false;

		model.SetPosition(2.0f, 0.0f, 0.0f);

		//Frank Luna
		//skull.Init(device.Get());
		//shapes.Init(device.Get());
		//hills.Init(device.Get());
		//wavesDemo.Init(device.Get());
		//box.Init(device.Get());
		//boxExtended.Init(device.Get());
		//pyramid.Init(device.Get());

		//Chapter 7
		//lightingDemo.Init(device.Get());
		//litSkullDemo.Init(device.Get());

		//Chapter 8
		//crateDemo.Init(device.Get());
		texturedHillsAndWavesDemo.Init(device.Get());
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}