---
layout: default
title: "2. Dx11 Init & imgui 써보기"
parent: "(Graphics🖼)"
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/master/02)

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

## Dx11 Init 절차 보기

```cpp
#include <d3d11.h>          // d3d11기능을 사용하겠다.
#include <d3dcompiler.h>    // PS, VS.hlsl을 컴파일 할때 사용된다.

// Init

/* ************************
* SwapChain Description 작성 *
************************** */
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



/* ************************
* SwapChain 및 Device 생성 *
************************** */
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



/* ************************
* RenderTarget 생성 *
    * 결국 그림은 BackBuffer(RenderTarget)에 그려지고
    * D3D에게 BackBuffer를 그려달라고 명령하는 형태로 진행된다.
************************** */
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

/*
void Render()
{
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    deviceContext->RSSetViewports(1, &viewport);
    deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
*/
```

🎈 **(간단 설명)** SwapChain으로 부터 `pBackBuffer`를 가져와 `renderTargetView`을 만드는게 이상하게 느껴질 수 있는데<br>
🎈 이렇게 생각하면 편하다 `ID3D11Texture2D* pBackBuffer;`는 포인터 주소이고 SwapChain에서 관리하는 백퍼터의 주소를 `ID3D11Texture2D* pBackBuffer;`로 가져온다.<br>
🎈 단순 주소만으론 이미지를 그릴 수 없으니 View를 통해 그리는데 그 View는 `CreateRenderTargetView`를 통해 받아온다.

```cpp
/* ************************
* ViewPort를 지정 *
    * 윈도우(HWND)에 어느영역에 그림을 그리나를 의미
************************** */
ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
viewport.TopLeftX = 0;
viewport.TopLeftY = 0;
viewport.Width = float(width);
viewport.Height = float(height);
viewport.MinDepth = 0.0f;
viewport.MaxDepth = 1.0f;//Note: important for depth buffering
deviceContext->RSSetViewports(1, &viewport);
```

🎈 **(간단 설명)** `D3D11_VIEWPORT` - 백버퍼에 랜더링할 영역을 나타낸다.<br>
🎈 여기서 드는 **의문 - 1**? 랜더링을 어디하나? -> Device에 넘겨준 hwnd에 랜더링을 한다. -> 가령 hwnd를 그림판에 할 경우 아래와 같이 된다.<br>

```cpp
HWND hwnd = (HWND)strtol("001708E2"/* 메모장의 hwnd */, NULL, 16);

// ...

DXGI_SWAP_CHAIN_DESC swapChainDesc;
ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
swapChainDesc.BufferDesc.Width = width;
swapChainDesc.BufferDesc.Height = height;
swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
swapChainDesc.BufferCount = 2;
swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; 
swapChainDesc.OutputWindow = hwnd;  // 메모장의 hwnd를 넘겨 Device자체를 메모장에 만든다 가정하자
swapChainDesc.SampleDesc.Count = 1; 
swapChainDesc.Windowed = TRUE;
swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
```

🎈 아래처럼 메모장에 그려지게 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-1-2.png"/>
</p>

🎈 여기서 드는 **의문 - 2**? 백버퍼에 랜더링할 영역이라 했는데 랜더링 영역을 변경할 수 있나?

```cpp
ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
// 이렇게 하면 위 그림처럼 검은색(랜더링되지않음)이 된다.
viewport.TopLeftX = 100;
viewport.TopLeftY = 100;
viewport.Width = float(width);
viewport.Height = float(height);
viewport.MinDepth = 0.0f;
viewport.MaxDepth = 1.0f;
deviceContext->RSSetViewports(1, &viewport);
```

```cpp
/* ************************
* 쉐이더 생성 *
************************** */
void InitShaders()
{
    // ID3DBlob - 범용 메모리 버퍼를 나타내는 형식이라고 한다(아직은 잘 모르겠음...)
    ID3DBlob* vertexBlob = nullptr;
    ID3DBlob* pixelBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    // .hlsl라는 쉐이더 파일을 읽어서 Vertex, Pixel 쉐이더를 컴파일한다.
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

    // .hlsl파일을 바탕으로 Device에 생성된 메모리 버퍼를 등록한다 생각하자
    device->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), NULL, &vertexShader);
    device->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, &pixelShader);

    // Dx12과 다른데 CPU의 데이터를 GPU로 넘기기 위해 사용됨(이후에 자세한 설명이 추가될 듯.?)
    // Create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    device->CreateInputLayout(ied, 2, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &layout);
    deviceContext->IASetInputLayout(layout);
}
```

```cpp
// 여기도 크게 중요하지 않아 일단 이런게 있다고 생각만 하자

/* **********************
* 텍스쳐(Texture)
    * GPU에 이미지를 저장해 두고 이리저리 붙이는것을 의미
************************* */
// Create texture and rendertarget
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
```

```cpp
D3D11_TEXTURE2D_DESC textureDesc;
ZeroMemory(&textureDesc, sizeof(textureDesc));
textureDesc.MipLevels = textureDesc.ArraySize = 1;
textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
textureDesc.SampleDesc.Count = 1;
textureDesc.Usage = D3D11_USAGE_DYNAMIC;
textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
textureDesc.MiscFlags = 0;
textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
// canvasWidth x canvasHeight의 Texture를 하나 생성한다.
textureDesc.Width = canvasWidth;
textureDesc.Height = canvasHeight;

device->CreateTexture2D(&textureDesc, nullptr, &canvasTexture);

if (canvasTexture)
{
    // device를 통해 shader resource view를 생성
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

// ...

// 픽셀 쉐이더 리소스로 넘기게 된다.
deviceContext->PSSetShaderResources(0, 1, &canvasTextureView);

// ...

// update
D3D11_MAPPED_SUBRESOURCE ms;
deviceContext->Map(canvasTexture, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
memcpy(ms.pData, pixels.data(), pixels.size() * sizeof(Vec4));
deviceContext->Unmap(canvasTexture, NULL);
```

```cpp
// vertext와 index 생성

// Create a vertex buffer
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
```