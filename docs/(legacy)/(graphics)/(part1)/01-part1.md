---
layout: default
title: "01. 환경 설정"
parent: "(Graphics Basic Concept)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 환경

* Visual Studio 2022
* DirectX 11

---

## vcpkg를 이용한 imgui 설치

```bash
$ git clone https://github.com/microsoft/vcpkg
$ cd .\vcpkg
$ .\bootstrap-vcpkg.bat
# pc에 vcpkg설치
$ .\vcpkg search imgui
# 설치가능(다운가능)한 imgui관련 패키지를 보여달라
$ .\vcpkg install imgui[win32-binding,dx11-binding]:x64-windows
# imgui를 win32, dx11용으로 다운받아달라
$ .\vcpkg integrate install 
# vs에서 imgui를 사용하게 준비해 달라
```

* 설치된 **imgui 라이브러리**를 프로젝트에 링킹시키면 됨.

---

## Example Project 살펴보기

* 실행해 보면 아래와 같이 뜬다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-1-2.png"/>
</p>

---

* [Clone Code 🌎](https://github.com/Arthur880708/DirectX11-Examples/tree/master/02)

## imgui 사용하기

```cpp
// include 확인
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

// ...

// Init
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO(); (void)io;
io.DisplaySize = ImVec2(width, height);
ImGui::StyleColorsLight();

// Setup Platform/Renderer backends
ImGui_ImplDX11_Init(example->device/*DxDevice*/, example->deviceContext/*DxContext*/);
ImGui_ImplWin32_Init(hwnd/*window handle*/);

// ...

// Render
ImGui_ImplDX11_NewFrame();
ImGui_ImplWin32_NewFrame();
ImGui::NewFrame();
ImGui::Begin("Background Color");
ImGui::SliderFloat3("RGB(0.0->1.0)", canvasColor, 0.0f, 1.0f);
ImGui::End();
ImGui::Render();

ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

// ...

// Cleanup
ImGui_ImplDX11_Shutdown();
ImGui_ImplWin32_Shutdown();
ImGui::DestroyContext();
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-1-1.png"/>
</p>

---

## Project 대략적 분석

```cpp
// ...

class Example
{
public:
	Example(HWND window, int width, int height, int canvasWidth, int canvasHeight)
	{
		Initialize(window, width, height, canvasWidth, canvasHeight);
	}

	// https://docs.microsoft.com/en-us/windows/win32/direct3d11/how-to--compile-a-shader
  // 쉐이더 준비
	void InitShaders()
	{
        /* **********************
        ** 쉐이더를 준비하다보면 Blob가 나오는데 **
        ** 빌드된 바이너리를 담는 통이라 생각하자 **
        ** (실제로 쓰이지는 않음.) **
        ** SwapChain 생성 **
        ********************** */

		ID3DBlob* vertexBlob = nullptr;
		ID3DBlob* pixelBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;

		if (FAILED(D3DCompileFromFile(L"VS.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &vertexBlob, &errorBlob)))
		{
			if (errorBlob) {
				std::cout << "Vertex shader compile error\n" << (char*)errorBlob->GetBufferPointer() << std::endl;
			}
		}

		if (FAILED(D3DCompileFromFile(L"PS.hlsl", 0, 0, "main", "ps_5_0", 0, 0, &pixelBlob, &errorBlob)))
		{
			if (errorBlob) {
				std::cout << "Pixel shader compile error\n" << (char*)errorBlob->GetBufferPointer() << std::endl;
			}
		}

    // 쉐이더를 빌드해 달라
		device->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), NULL, &vertexShader);
		device->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, &pixelShader);


		// Create the input layout object
    // 쉐이더에게 아래와 같이 데이터를 넘길 예정이다.
		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		device->CreateInputLayout(ied, 2, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &layout);
    // input assembler에게 넘긴다.
		deviceContext->IASetInputLayout(layout);
	}
```

```cpp
	void Initialize(HWND window, int width, int height, int canvasWidth, int canvasHeight)
	{
		this->canvasWidth = canvasWidth;
		this->canvasHeight = canvasHeight;

        /* **********************
        ** SwapChain 생성 **
        ********************** */
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferDesc.Width = width;                   // set the back buffer width
		swapChainDesc.BufferDesc.Height = height;                 // set the back buffer height
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
		swapChainDesc.BufferCount = 2;                                   // one back buffer
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // how swap chain is to be used
		swapChainDesc.OutputWindow = window;                               // the window to be used
		swapChainDesc.SampleDesc.Count = 1;                              // how many multisamples
		swapChainDesc.Windowed = TRUE;                                   // windowed/full-screen mode
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

		const D3D_FEATURE_LEVEL featureLevelArray[1] = { D3D_FEATURE_LEVEL_11_0};
		if (FAILED(D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			createDeviceFlags,
			featureLevelArray,
			1,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&swapChain,
			&device,
			NULL,
			&deviceContext)))
		{
			std::cout << "D3D11CreateDeviceAndSwapChain() error" << std::endl;
		}

		// CreateRenderTarget
        /* **********************
        ** BackBuffer의 RenderTarget 생성 **
        ********************** */
		ID3D11Texture2D* pBackBuffer;
		swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		if (pBackBuffer)
		{
			device->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
			pBackBuffer->Release();
		}
		else
		{
			std::cout << "CreateRenderTargetView() error" << std::endl;
			exit(-1);
		}

		// Set the viewport
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.TopLeftX = 100;
		viewport.TopLeftY = 100;
		viewport.Width = float(width);
		viewport.Height = float(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;//Note: important for depth buffering
		deviceContext->RSSetViewports(1, &viewport);

		InitShaders();

		// Create texture and rendertarget
        /* **********************
        ** Pixel Sampler 생성 **
        ********************** */
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // D3D11_FILTER_MIN_MAG_MIP_LINEAR
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		//Create the Sample State
		device->CreateSamplerState(&sampDesc, &colorSampler);


        /* **********************
        ** 그림을 그려넣을 RenderTarget(Texture) 생성 **
        ********************** */
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.MipLevels = textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DYNAMIC;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.MiscFlags = 0;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		textureDesc.Width = canvasWidth;
		textureDesc.Height = canvasHeight;

		device->CreateTexture2D(&textureDesc, nullptr, &canvasTexture);

		if (canvasTexture)
		{
			device->CreateShaderResourceView(canvasTexture, nullptr, &canvasTextureView);

			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
			renderTargetViewDesc.Format = textureDesc.Format;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;

			device->CreateRenderTargetView(canvasTexture, &renderTargetViewDesc, &canvasRenderTargetView);
		}
		else
		{
			std::cout << "CreateRenderTargetView() error" << std::endl;
		}

		// Create a vertex buffer
        /* **********************
        ** Vertex 생성 **
        ********************** */
		{
			const std::vector<Vertex> vertices =
			{
				{ { -1.0f, -1.0f, 0.0f, 1.0f }, { 0.f, 1.f },},
				{ {  1.0f, -1.0f, 0.0f, 1.0f }, { 1.f, 1.f },},
				{ {  1.0f,  1.0f, 0.0f, 1.0f }, { 1.f, 0.f },},
				{ { -1.0f,  1.0f, 0.0f, 1.0f }, { 0.f, 0.f },},
			};

			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
			bufferDesc.ByteWidth = UINT(sizeof(Vertex) * vertices.size());             // size is the VERTEX struct * 3
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
			bufferDesc.StructureByteStride = sizeof(Vertex);

			D3D11_SUBRESOURCE_DATA vertexBufferData = { 0, };
			vertexBufferData.pSysMem = vertices.data();
			vertexBufferData.SysMemPitch = 0;
			vertexBufferData.SysMemSlicePitch = 0;

			const HRESULT hr = device->CreateBuffer(&bufferDesc, &vertexBufferData, &vertexBuffer);
			if (FAILED(hr)) {
				std::cout << "CreateBuffer() failed. " << std::hex << hr << std::endl;
			};
		}

		// Create an index buffer
        /* **********************
        ** Index Buffer 생성 **
        ********************** */
		{
			const std::vector<uint16_t> indices =
			{
				3,1,0,
				2,1,3,
			};

			indexCount = UINT(indices.size());

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
			bufferDesc.ByteWidth = UINT(sizeof(uint16_t) * indices.size());
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;       // use as a vertex buffer
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
			bufferDesc.StructureByteStride = sizeof(uint16_t);

			D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
			indexBufferData.pSysMem = indices.data();
			indexBufferData.SysMemPitch = 0;
			indexBufferData.SysMemSlicePitch = 0;

			device->CreateBuffer(&bufferDesc, &indexBufferData, &indexBuffer);
		}
	}
```

```cpp
	void Update()
	{
		std::vector<Vec4> pixels(canvasWidth * canvasHeight, Vec4{0.8f, 0.8f, 0.8f, 1.0f});
		pixels[0 + canvasWidth * 0] = Vec4{ 1.0f, 0.0f, 0.0f, 1.0f };
		pixels[1 + canvasWidth * 0] = Vec4{ 1.0f, 1.0f, 0.0f, 1.0f };

		// Update texture buffer
        /* **********************
        ** RenderTarget Texture에 그림을 넣는다 **
        ********************** */
		D3D11_MAPPED_SUBRESOURCE ms;
		deviceContext->Map(canvasTexture, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, pixels.data(), pixels.size() * sizeof(Vec4));
		deviceContext->Unmap(canvasTexture, NULL);
	}
```

```cpp
	void Render()
	{
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		deviceContext->RSSetViewports(1, &viewport);
		deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
		deviceContext->ClearRenderTargetView(renderTargetView, clearColor);

		// set the shader objects
		deviceContext->VSSetShader(vertexShader, 0, 0);
		deviceContext->PSSetShader(pixelShader, 0, 0);

		// select which vertex buffer to display
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// https://github.com/Microsoft/DirectXTK/wiki/Getting-Started
		// https://github.com/microsoft/Xbox-ATG-Samples/tree/main/PCSamples/IntroGraphics/SimpleTexturePC
		deviceContext->PSSetSamplers(0, 1, &colorSampler);//TODO: samplers to array
    // Pixel Sampler에 Texture를 넘긴다
    deviceContext->PSSetShaderResources(0, 1, &canvasTextureView);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->DrawIndexed(indexCount, 0, 0);
	}
```