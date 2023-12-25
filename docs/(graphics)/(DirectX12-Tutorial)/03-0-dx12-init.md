---
layout: default
title: "03-00. 삼각형을 그려보자"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/3)

## Mesh 클래스

* 정점정보를 관리하는 클래스를 만들어보자
    * 현재는 삼각형만 사용하는데 이후에는 복잡한 구조를 넣게된다.

```cpp
class Mesh 
{ 
public: 
    /*
    struct Vertex
    {
        Vec3 pos;   // 위치정보
        Vec4 color; // 색상정보
    }
    */
    void Init(vector<Vertex>& vec); 
    void Render(); 

private: 
    ComPtr<ID3D12Resource>      _vertexBuffer; 
    // GPU에 할당할 Mesh 메모리(리소스)
    D3D12_VERTEX_BUFFER_VIEW    _vertexBufferView = {}; 
    // 할당된 메모리(리소스)의 핸들(View)
    uint32 _vertexCount = 0; 
};
```

```cpp
void Mesh::Init(vector<Vertex>& vec)
{
    _vertexCount = static_cast<uint32>(vec.size());
    uint32 bufferSize = _vertexCount * sizeof(Vertex);

    // GPU에 업로드용 메모리를 할당해 달라
    D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);   // 업로드용
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    // GPU의 공간을 빌린다.
    DEVICE->CreateCommittedResource(
        &heapProperty,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&_vertexBuffer));  // _vertexBuffer가 생성됨, _vertexBuffer : GPU공간의 메모리를 의미

    // _vertexBuffer는 GPU공간이라 그냥 복사가 불가능 아래의 과정이 필요하다
    void* vertexDataBuffer = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    _vertexBuffer->Map(0, &readRange, &vertexDataBuffer);   // GPU공간에 접근할 권한을 잠깐 갖는다.
    ::memcpy(vertexDataBuffer, &vec[0], bufferSize);        // 메모리 복사 후
    _vertexBuffer->Unmap(0, nullptr);                       // 권한을 다시 돌려줌

    // Initialize the vertex buffer view.
    _vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
    _vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
    _vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기   
}

void Mesh::Render()
{
    // 커멘드 리스트로 그리기를 요청함을 주목하자
    CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);  // Mesh가 어떤형태인지(기본은 삼각형(TRIANGLELIST))
    
    // 참고로 View(_vertexBufferView)를 통해 명령을 전달함을 확인하자
    CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)
    CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0); // 그려줭
}
```

---

## Shader 클래스

* 정점사이를 어떻게 채워넣을지(그릴지) 관리

```cpp
// 쉐이더 파일(default.hlsli)

// 자료형을 정의
// Vertext 인풋
struct VS_IN
{
    // {자료형} {변수명} : {Semantic(정의형태?)}; // Semantic - D3D12_INPUT_ELEMENT_DESC로 어떻게 정의되었는지 GPU에게 알리고 없다면 무시된다.
    float3 pos : POSITION;
    float4 color : COLOR;
};

// Vertext 아웃풋
struct VS_OUT
{
    float4 pos : SV_Position;   // SV = System Value, 시스템상 정의된 Position
    float4 color : COLOR;
};

// Vertex 쉐이더는 위치이동없이 그대로 리턴해 달라
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = float4(input.pos, 1.f);
    output.color = input.color;

	// 입력들어온데로 거의 그대로 넘긴다
    return output;
}

// Pixel 쉐이더는 들어온 컬러 그대로 리턴해달라
// SV_Target - 랜더타겟으로 Pixel 쉐이더 결과를 넘겨라
float4 PS_Main(VS_OUT input) : SV_Target
{
	// 역시 들어온데로 그대로 넘긴다
    return input.color;
}
```

* [System Value(_SV) Semantic 🌍](https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics?redirectedfrom=MSDN#system-value-semantics)

```cpp
#pragma once
// [일감 기술서] 외주 인력(GPU)들이 뭘 해야할지 기술
class Shader
{
public:
    void Init(const wstring& path);     // 쉐이더 파일을 받아야 한다.
    void Update();

private:
    void CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode);
    void CreateVertexShader(const wstring& path, const string& name, const string& version);
    void CreatePixelShader(const wstring& path, const string& name, const string& version);
    
private:
    // _vsBlob, _psBlob - Vertex, Shader 코드를 파싱 후 블럭화 하여 넣어둔다
    ComPtr<ID3DBlob>                    _vsBlob;
    ComPtr<ID3DBlob>                    _psBlob;
    ComPtr<ID3DBlob>                    _errBlob;

    ComPtr<ID3D12PipelineState>         _pipelineState;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC  _pipelineDesc = {};
};
```

```cpp
#include "pch.h"
#include "Shader.h"
#include "Engine.h"

void Shader::Init(const wstring& path)
{
	CreateVertexShader(path, "VS_Main", "vs_5_0");  
	// 정점을 어떻게 변경할지 정보
	// Vertex Shader 쉐이더파일 에서 VS_Main을 부른다.
	// vs_5_0 : 쉐이더 버전을 지정(받아들이자)

	CreatePixelShader(path, "PS_Main", "ps_5_0");   
	// Pixel을 어떻게 그릴지 정보
	// Pixel Shader 쉐이더파일 에서 PS_Main을 부른다.
	// ps_5_0 : 쉐이더 버전을 지정(받아들이자)

	// 생성된 Vertex, Pixel 쉐이더는 _pipelineDesc에 담겨있다.

    // 어떤 자료형을 쉐이더에서 사용할지 넘겨준다
	D3D12_INPUT_ELEMENT_DESC desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	_pipelineDesc.InputLayout = { desc, _countof(desc) };
	_pipelineDesc.pRootSignature = ROOT_SIGNATURE.Get();        // RootSignature를 넣는다

	_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	_pipelineDesc.DepthStencilState.DepthEnable = FALSE;
	_pipelineDesc.DepthStencilState.StencilEnable = FALSE;
	_pipelineDesc.SampleMask = UINT_MAX;
	_pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	_pipelineDesc.NumRenderTargets = 1;
	_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	_pipelineDesc.SampleDesc.Count = 1;

	// Device에서 PipelineState를 생성함을 기억
	DEVICE->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));
}
```

```cpp
void Shader::CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode)
{
	uint32 compileFlag = 0;
#ifdef _DEBUG
	compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (FAILED(::D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, name.c_str(), version.c_str(), compileFlag, 0, &blob, &_errBlob)))
	{
		::MessageBoxA(nullptr, "Shader Create Failed !", nullptr, MB_OK);
	}

	shaderByteCode = { blob->GetBufferPointer(), blob->GetBufferSize() };
}

void Shader::CreateVertexShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, _vsBlob, _pipelineDesc.VS);
}

void Shader::CreatePixelShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, _psBlob, _pipelineDesc.PS);
}
```

```cpp
void Shader::Update()
{
	CMD_LIST->SetPipelineState(_pipelineState.Get());
	// 쉐이더가 변경되어 파이프라인의 상태가 변경되었음을 알린다. 
}
```

* 삼각형 Vertex 전달

```cpp
void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

	vector<Vertex> vec(3);
	vec[0].pos = Vec3(0.f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);

	vec[1].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.0f, 0.f, 1.f);

	vec[2].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);

    /*
    Vec3(x, y, z)이고 가운데가 0,0임을 기억하자

    ------------------------
    |                      |
    |        (0,.5)        |
    |        (0,0)         |
    |(-.5,-.5)    (.5,.5)  |
    |                      |
    ------------------------
    */

	mesh->Init(vec);

	shader->Init(L"..\\Resources\\Shader\\default.hlsli");

	GEngine->GetCmdQueue()->WaitSync();
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-3-1.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>
