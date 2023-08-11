---
layout: default
title: "01. Project Settings"
parent: "(DirectX11 2D Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Init Project

* 프로젝트 세팅에 몇 가지가 필요한데... 다 설명하기엔 너무 기초적이고 간단하기에 그냥 아래 코드를 받자
    * [Get Code 🌎](https://github.com/Arthur880708/Direct11-Tutorials/tree/1.Init)

---

## Init DirectX-11

* [Get Code 🌎]()

```cpp
// 사실상 하나하나 다 알필요는 없고
// 이런식으로 Device와 Swapchain을 만든다 정도만 알자
void Game::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	{
		desc.BufferDesc.Width = _width;
		desc.BufferDesc.Height = _height;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;
		desc.OutputWindow = _hwnd;
		desc.Windowed = true;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		_swapChain.GetAddressOf(),
		_device.GetAddressOf(),
		nullptr,
		_deviceContext.GetAddressOf()
	);
    // (TIPS) .Get() -> 스마트 포인터의 주소를 얻어옴
        //    .GetAddressOf() -> 생 포인터 주소를 얻어옴

	CHECK(hr);
}
```

```cpp
// 다음은 RenderTargetView를 만드는 부분인데
    // DX12 View == DescriptorHeap
void Game::CreateRenderTargetView()
{
	HRESULT hr;

    // backbuffer의 Texture를 준비하고
	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
    // swapchain에게 BackBuffer를 달라고 요청한다
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	CHECK(hr);

    // 그렇게 얻어온 backbuffer를 RenderTargetView로 쓰겠다 선언!
	_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	CHECK(hr);
}
```

* Q) 그냥 BackBuffer에 쓰면되지 왜 RenderTargetView를 이용하나?
    * A) 설명하자면 긴데 ... 그냥 backbuffer에 쓴다는게 결국 pixel을 모두 계산한 값을 backbuffer(texture)에 넣는다는 말 이다.
    * 그럼 결론적으로 모든 3D연산이 된 값을 사용자(개발자)가 미리 알고있어야 한다는 말인데 그럴꺼면 Graphic API를 쓸 이유가 없겠지? 이런걸 계산해 주는게 DirectX니깐!
    * **결론적으로** 결국 하고자 하는것은 화면에 그리는 것이며, 그 그리기 위한 texture를 개발자가 직접 그릴순 없으니 rendertargetview라는 애를 통해서 그린다

```cpp
// 그리기 시작전 초기화

void Game::RenderBegin()
{
    // 여기도 주목해야할 점이
    // 결국 그려달라 명령은 device를 통해한다
        // 그리고 그 device에게 명령을 내리는 애는 deviceContext이다
    // deviceContext에서 RenderTarget을 Set함을 기억하자
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);
	_deviceContext->RSSetViewports(1, &_viewport);
}
```

---

## 삼각형을 띄워보자

### Vertex만들기

```cpp
void Game::CreateGeometry()
{
    // 우선, Vertex를 하나 만들자
	{
        // Data를 아래와 같이 구성하고
		_vertices.resize(3);

		_vertices[0].position = Vec3(-0.5f, -0.5f, 0.f);
		_vertices[0].color = Color(1.f, 0.f, 0.f, 1.f);
		_vertices[1].position = Vec3(0.f, 0.5f, 0.f);
		_vertices[1].color = Color(0.f, 1.f, 0.f, 1.f);
		_vertices[2].position = Vec3(0.5f, -0.5f, 0.f);
		_vertices[2].color = Color(0.f, 0.f, 1.f, 1.f);
	}
	
	{
        // VertexBuffer를 만든다
            // ??? _vertices자체가 buffer인데 이걸 왜 만들어?
            // _vertices는 CPU용 buffer고 GPU용 buffer를 만든다 생각
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
        // D3D11_USAGE_IMMUTABLE - GPU만 읽을수 있다
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = (uint32)(sizeof(Vertex) * _vertices.size());

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _vertices.data();

		_device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
	}
}
```

* 여기서 주목해야 할 점은 `_vertices`의 구조가 아래와 같이 position, color로 구성되어 있는데

```cpp
struct Vertex
{
	Vec3 position;
	Color color;
};
```

* 이걸 어떻게 GPU는 알까? 이다.
* 정답은 개발자가 알려줘야한다

```cpp
void Game::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
        // 처음이 position, 다음이 color이다
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const int32 count = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

    // Input의 Layout을 만들어 두고
	_device->CreateInputLayout(layout, count, _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), _inputLayout.GetAddressOf());
}

```

```cpp
void Game::Render()
{
	RenderBegin();

	{
		uint32 stride = sizeof(Vertex);
		uint32 offset = 0;
		
		// IA
            // 여기서 vertextBuffer를 넣어준다
		_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset); 
            // Layout은 이렇습니다! 알린다
		_deviceContext->IASetInputLayout(_inputLayout.Get());
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS
		_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);

		// RS

		// PS
		_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);

		// OM

		_deviceContext->Draw(_vertices.size(), 0);
	}

	RenderEnd();
}
```

---

### Shader 만들기

* Default.hlsl을 아래와 같이 만든다

```cpp
struct VS_INPUT
{
    // : 뒤에 나오는 이름이 InputLayout에서 지정한 이름이다
	float4 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
    // SV_POSITION는 pixel shader에 꼭 있어야 하는 변수이다
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

// IA - VS - RS - PS - OM
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = input.position;
	output.color = input.color;

	return output;
}

// SV_Target - PS 얘를 RenderTarget하는데 쓰겠다. 참고로 번호도 지정해서 다중 랜더링 가능
float4 PS(VS_OUTPUT input) : SV_Target
{
	
	
	return input.color;
}
```

* 아래와 같이 shader를 만들고, 사용하면 끝!

```cpp
void Game::CreateVS()
{
	LoadShaderFromFile(L"Default.hlsl", "VS", "vs_5_0", _vsBlob);
 	HRESULT hr = _device->CreateVertexShader(_vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), nullptr, _vertexShader.GetAddressOf());
	CHECK(hr);
}

void Game::CreatePS()
{
	LoadShaderFromFile(L"Default.hlsl", "PS", "ps_5_0", _psBlob);
	HRESULT hr = _device->CreatePixelShader(_psBlob->GetBufferPointer(), _psBlob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());
	CHECK(hr);
}

void Game::LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob)
{
	const uint32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	HRESULT hr = ::D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		name.c_str(),
		version.c_str(),
		compileFlag,
		0,
		blob.GetAddressOf(),
		nullptr);

	CHECK(hr);
}
```

---

## Index Buffer

* **Index Buffer**는 왜 필요할까?
    * 가령 사각형을 그린다 가정해 보자 Vertex는 몇개가 필요할까?

```

1 -----  2
|     /  |
|   /    |
| /      |
3 ------ 4

```

* 위 그림과 같이 4개만 쓰면 최선일 것이다.
* 그런데 2, 3은 재활용 해야하기에 GPU에게 Index를 넘겨 어떤게 재활용되는 Vertex인지 알려야한다

```cpp
void Game::CreateGeometry()
{
	// ...

/*
1 -----  3
| \      |
|   \    |
|     \  |
0 ------ 2

??? 왜 0번이 왼쪽아래지? -> Vertex배열이 들어간 순서임 큰 의미없음.
*/

	// Index
	{
        // 0 1 2 / 2 1 3 두 개를 만들어 주세요!
		_indices = {0, 1, 2, 
                    2, 1, 3};
	}

	// IndexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = (uint32)(sizeof(uint32) * _indices.size());

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _indices.data();

		HRESULT hr = _device->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
		CHECK(hr);
	}
}
```

---

## Texture(이미지) 붙이기

### uv좌표계

* 우선 uv좌표계에 대해 알아야 한다
* 사실 간단 x, y 좌표를 0~1사이로 매핑한 좌표계

```
00 -----  10
|         |
|         |
|         |
01 ------ 11
```

* 이제 Vertex에 uv좌표계도 넣어보자

```cpp
struct Vertex
{
	Vec3 position;
	//Color color;
	Vec2 uv;
    // 이게 무슨의미냐? 해당 Vertex가 uv상에 어떤점에 존재한다를 알린다
};
```

* ??? 이걸 뭐에써?
    * shader 코드를 보면 명확해 진다

```cpp
float4 PS(VS_OUTPUT input) : SV_Target
{
    // texture에 특정 지점의 색을 sampling!
	float4 color = texture0.Sample(sampler0, input.uv);
	
	return color;
}
```

---

### Texture 가져오기

* 우선 .png파일을 읽어온다

```cpp
void Game::CreateSRV()
{
	DirectX::TexMetadata md;
	DirectX::ScratchImage img;
    {
        HRESULT hr = ::LoadFromWICFile(L"Skeleton.png", WIC_FLAGS_NONE, &md, img);
        CHECK(hr);

        // 쉐이더에게 념겨주는 Resource라 이름이 ShaderResourceView!
        hr = ::CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourveView.GetAddressOf());
        CHECK(hr);
    }

    {
        hr = ::LoadFromWICFile(L"Golem.png", WIC_FLAGS_NONE, &md, img);
        CHECK(hr);

        hr = ::CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourveView2.GetAddressOf());
        CHECK(hr);
    }
}
```

* 이제 pixelshader에게 resource를 넘긴다

```cpp
void Game::Render()
{
	// ...

    // PS
    _deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
    _deviceContext->PSSetShaderResources(0, 1, _shaderResourveView.GetAddressOf());
    _deviceContext->PSSetShaderResources(1, 1, _shaderResourveView2.GetAddressOf());
    _deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf());

	// ...
}
```

---

## Constant Buffer

* Vertex의 위치를 실시간으로 바꿀순 없을까?
* 매번 Vertex를 만들어 넣어야 하나?

```cpp
void Game::CreateGeometry()
{
	{
		_vertices.resize(3);
		_vertices[0].position = Vec3(-0.5f, -0.5f, 0.f);
		_vertices[0].color = Color(1.f, 0.f, 0.f, 1.f);
		_vertices[1].position = Vec3(0.f, 0.5f, 0.f);
		_vertices[1].color = Color(0.f, 1.f, 0.f, 1.f);
		_vertices[2].position = Vec3(0.5f, -0.5f, 0.f);
		_vertices[2].color = Color(0.f, 0.f, 1.f, 1.f);
	}
	
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
        // 기존에 Vertex Buffer를 만들며 D3D11_USAGE_IMMUTABLE를 선언했는데
        // 생각해보면 GPU에서만 읽을수 있고(쓸수도 없음) CPU는 접근도 불가능한
        // 이런 애를 왜 만들었을까?

        // 정답은 GPU메모리에 올릴때는 어떤 물체의 기하학적인 모양 그 자체를 보낸다
        // 일단 GPU메모리에 올려두고 다른값(Constant Buffer)를 이용해 편집을하게된다.
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = (uint32)(sizeof(Vertex) * _vertices.size());

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _vertices.data();

		_device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
	}
}
```

* 그럼 상수 버퍼(Constant Buffer)를 만들어 보자

```cpp
// 우선 Shader에서는 Constant buffer를 받을 준비를 하고
cbuffer TransformData : register(b0)
{
	float4 offset;
}
```

```cpp
void Game::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC; // CPU_Write + GPU_Read
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(TransformData);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = _device->CreateBuffer(&desc, nullptr, _constantBuffer.GetAddressOf());
	CHECK(hr);
}
```

```cpp
void Game::Update()
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	ZeroMemory(&subResource, sizeof(subResource));
	
	_deviceContext->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	::memcpy(subResource.pData, &_transformData, sizeof(_transformData));
	_deviceContext->Unmap(_constantBuffer.Get(), 0);
}
```

---

## RasterizerState

```cpp
void Game::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

    // 이 옵션에 따라 다르게 나타난다
        // FillMode - 어떻게 채울것인가
	desc.FillMode = D3D11_FILL_SOLID;

        // CullMode - Culling을 어떻게 넣을것인가
        // FrontCounterClockwise시계방향으로 Back만 등등
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = false;

	HRESULT hr = _device->CreateRasterizerState(&desc, _rasterizerState.GetAddressOf());
	CHECK(hr);
}
```

---

## SampleState

```cpp
void Game::CreateSamplerState()
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.BorderColor[0] = 1;
	desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0;
	desc.BorderColor[3] = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxAnisotropy = 16;
	desc.MaxLOD = FLT_MAX;
	desc.MinLOD = FLT_MIN;
	desc.MipLODBias = 0.0f;

	_device->CreateSamplerState(&desc, _samplerState.GetAddressOf());
}
```

---

## BlendState

```cpp
void Game::CreateBlendState()
{
    // Vertex의 Alpha값에 따라 어떻게 반영될지 결정
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr = _device->CreateBlendState(&desc, _blendState.GetAddressOf());
	CHECK(hr);
}
```