---
layout: default
title: "03. 삼각형을 그려보자"
parent: "(DirectX12 25년 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 사전지식1) Pipeline State란?

* D3D12에서 **Pipeline State Object(PSO)** 는 그래픽스/컴퓨트 파이프라인의 모든 고정 및 프로그램 가능한 상태를 하나의 불변 객체로 묶은 것
* GPU가 명령을 실행할 때 필요한 설정을 최소한으로 전환하고, 드라이버 오버헤드를 줄이기 위해 도입

> 우선 코드를 보면 좀 더 이해가 된다.

```cpp
ComPtr<ID3D12PipelineState> CreateGraphicsPso(
    ID3D12Device* device,
    DXGI_FORMAT rtv_format,
    DXGI_FORMAT dsv_format)
{
    // 1. 셰이더 컴파일
    ComPtr<ID3DBlob> vs = CompileShader(L"shader.hlsl", "VSMain", "vs_5_0");
    ComPtr<ID3DBlob> ps = CompileShader(L"shader.hlsl", "PSMain", "ps_5_0");
    if (!vs || !ps)
    {
        return nullptr;
    }

    // 2. Root Signature
    ComPtr<ID3D12RootSignature> root_signature = CreateSimpleRootSignature(device);
    if (!root_signature)
    {
        return nullptr;
    }

    // 3. Input Layout
    std::array<D3D12_INPUT_ELEMENT_DESC, 2> input_elements;
    D3D12_INPUT_LAYOUT_DESC input_layout = CreateSimpleInputLayout(input_elements);

    // 4. Rasterizer State
    D3D12_RASTERIZER_DESC rasterizer_desc{};
    rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK;
    rasterizer_desc.FrontCounterClockwise = FALSE;
    rasterizer_desc.DepthClipEnable = TRUE;

    // 5. Blend State
    D3D12_BLEND_DESC blend_desc{};
    blend_desc.AlphaToCoverageEnable = FALSE;
    blend_desc.IndependentBlendEnable = FALSE;
    auto& rt0 = blend_desc.RenderTarget[0];
    rt0.BlendEnable = FALSE;
    rt0.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // 6. Depth-Stencil State
    D3D12_DEPTH_STENCIL_DESC depth_stencil_desc{};
    depth_stencil_desc.DepthEnable = TRUE;
    depth_stencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depth_stencil_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depth_stencil_desc.StencilEnable = FALSE;

    // 7. PSO Desc
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
    pso_desc.pRootSignature = root_signature.Get();
    pso_desc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
    pso_desc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
    pso_desc.BlendState = blend_desc;
    pso_desc.SampleMask = UINT_MAX;
    pso_desc.RasterizerState = rasterizer_desc;
    pso_desc.DepthStencilState = depth_stencil_desc;
    pso_desc.InputLayout = input_layout;
    pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso_desc.NumRenderTargets = 1;
    pso_desc.RTVFormats[0] = rtv_format;
    pso_desc.DSVFormat = dsv_format;
    pso_desc.SampleDesc.Count = 1;
    pso_desc.SampleDesc.Quality = 0;

    ComPtr<ID3D12PipelineState> pso;
    HRESULT hr = device->CreateGraphicsPipelineState(
        &pso_desc,
        IID_PPV_ARGS(&pso));
    if (FAILED(hr))
    {
        return nullptr;
    }

    return pso;
}
```

### Pipeline State Object 종류

* Graphics Pipeline State
    * 그래픽 렌더링 시 필요한 대부분의 상태를 포함
* Compute Pipeline State
    * 컴퓨트 셰이더 실행에 필요한 정보만 포함

### Graphics Pipeline State 구성 요소

#### 1. 셰이더 프로그램

* VS (Vertex Shader)
* PS (Pixel Shader)
* GS (Geometry Shader, 선택)
* HS / DS (Tessellation Shader, 선택)
* MS / AS (Mesh / Amplification Shader, 선택)

#### 2. Input Layout (입력 레이아웃)

* 버텍스 버퍼의 메모리 구조를 정의합니다.

#### Rasterizer State

* 삼각형이 화면에 투영되는 방식 제어:
    * FillMode
    * CullMode
    * DepthBias
    * Conservative Rasterization 등

#### 4 Blend State

* 픽셀 출력과 기존 렌더 타깃 값의 혼합 방식 제어:
    * Src/Dst Blend
    * BlendOp
    * RenderTargetWriteMask 등

#### 5 Depth-Stencil State

* Depth/Stencil 테스트 및 갱신 설정:
    * DepthEnable / DepthFunc
    * StencilEnable / StencilOp
    * DepthWriteMask 등

#### 6 Render Target / Depth Format

* 렌더 타깃의 개수와 각각의 DXGI 포맷:
    * RTVFormats[8]
    * DSVFormat
    * SampleDesc

#### 7 Root Signature

* PSO가 사용하는 리소스 바인딩 규칙.
* PSO가 생성될 때 root signature가 고정됩니다.

---

## 사전지식2) RootSignature란?

### Root Signature란?

* Root Signature는 셰이더가 접근하는 GPU 리소스를 정의하는 규칙
* 셰이더에서 사용할 CBV, SRV, UAV, Sampler 등을 어떤 방식으로 바인딩할지를 명시하며,
* Pipeline State Object(PSO)의 일부로 고정

```cpp
D3D12_ROOT_PARAMETER params[2];

// 1) CBV root parameter
params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
params[0].Descriptor.ShaderRegister = 0; // b0
params[0].Descriptor.RegisterSpace = 0;
params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

// 2) Descriptor Table (SRV 0~3)
D3D12_DESCRIPTOR_RANGE range{};
range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
range.NumDescriptors = 4;
range.BaseShaderRegister = 0; // t0
range.RegisterSpace = 0;
range.OffsetInDescriptorsFromTableStart = 0;

D3D12_ROOT_DESCRIPTOR_TABLE table{};
table.NumDescriptorRanges = 1;
table.pDescriptorRanges = &range;

params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
params[1].DescriptorTable = table;
params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

// Root Signature 생성
D3D12_ROOT_SIGNATURE_DESC desc{};
desc.NumParameters = 2;
desc.pParameters = params;
desc.NumStaticSamplers = 0;
desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

ComPtr<ID3DBlob> blob, error;
HRESULT hr = D3D12SerializeRootSignature(
    &desc,
    D3D_ROOT_SIGNATURE_VERSION_1,
    &blob,
    &error);

ComPtr<ID3D12RootSignature> root_sig;
device->CreateRootSignature(
    0,
    blob->GetBufferPointer(),
    blob->GetBufferSize(),
    IID_PPV_ARGS(&root_sig));

```

---

## 자! 시작해 보자

> 위의 내용을 안다면 쉽게 읽힘

```cpp
ID3D12RootSignature* CBasicMeshObject::m_pRootSignature = nullptr;
ID3D12PipelineState* CBasicMeshObject::m_pPipelineState = nullptr;
DWORD CBasicMeshObject::m_dwInitRefCount = 0;

CBasicMeshObject::CBasicMeshObject()
{
}

BOOL CBasicMeshObject::Initialize(CD3D12Renderer* pRenderer)
{
	m_pRenderer = pRenderer;

	BOOL bResult = InitCommonResources();
	return bResult;
}
BOOL CBasicMeshObject::InitCommonResources()
{
	if (m_dwInitRefCount)
		goto lb_true;

	InitRootSinagture();
	InitPipelineState();

lb_true:
	m_dwInitRefCount++;
	return m_dwInitRefCount;
}
BOOL CBasicMeshObject::InitRootSinagture()
{
	ID3D12Device5* pD3DDeivce = m_pRenderer->INL_GetD3DDevice();

	// Create an empty root signature.
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* pSignature = nullptr;
	ID3DBlob* pError = nullptr;

	if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError)))
	{
		__debugbreak();
	}

	if (FAILED(pD3DDeivce->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature))))
	{
		__debugbreak();
	}
	if (pSignature)
	{
		pSignature->Release();
		pSignature = nullptr;
	}
	if (pError)
	{
		pError->Release();
		pError = nullptr;
	}
	return TRUE;
}
BOOL CBasicMeshObject::InitPipelineState()
{
	ID3D12Device5* pD3DDeivce = m_pRenderer->INL_GetD3DDevice();

	ID3DBlob* pVertexShader = nullptr;
	ID3DBlob* pPixelShader = nullptr;


#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	if (FAILED(D3DCompileFromFile(L"./Shaders/shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &pVertexShader, nullptr)))
	{
		__debugbreak();
	}
	if (FAILED(D3DCompileFromFile(L"./Shaders/shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pPixelShader, nullptr)))
	{
		__debugbreak();
	}


	// Define the vertex input layout.
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};


	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_pRootSignature;
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pPixelShader->GetBufferPointer(), pPixelShader->GetBufferSize());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	if (FAILED(pD3DDeivce->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pPipelineState))))
	{
		__debugbreak();
	}

	if (pVertexShader)
	{
		pVertexShader->Release();
		pVertexShader = nullptr;
	}
	if (pPixelShader)
	{
		pPixelShader->Release();
		pPixelShader = nullptr;
	}
	return TRUE;
}

BOOL CBasicMeshObject::CreateMesh()
{
	BOOL bResult = FALSE;
	ID3D12Device5* pD3DDeivce = m_pRenderer->INL_GetD3DDevice();

	// Create the vertex buffer.
	// Define the geometry for a triangle.
	BasicVertex Vertices[] =
	{
		{ { 0.0f, 0.25f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	const UINT VertexBufferSize = sizeof(Vertices);

	// Note: using upload heaps to transfer static data like vert buffers is not 
	// recommended. Every time the GPU needs it, the upload heap will be marshalled 
	// over. Please read up on Default Heap usage. An upload heap is used here for 
	// code simplicity and because there are very few verts to actually transfer.
	if (FAILED(pD3DDeivce->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(VertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pVertexBuffer))))
	{
		__debugbreak();
	}

	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin = nullptr;
	CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.

	if (FAILED(m_pVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin))))
	{
		__debugbreak();
	}
	memcpy(pVertexDataBegin, Vertices, sizeof(Vertices));
	m_pVertexBuffer->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	m_VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(BasicVertex);
	m_VertexBufferView.SizeInBytes = VertexBufferSize;

	bResult = TRUE;

lb_return:
	return bResult;
}
void CBasicMeshObject::Draw(ID3D12GraphicsCommandList* pCommandList)
{
	// set RootSignature
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);
   
	pCommandList->SetPipelineState(m_pPipelineState);
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
    pCommandList->DrawInstanced(3, 1, 0, 0);

}

void CBasicMeshObject::Cleanup()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}
	CleanupSharedResources();
}
void CBasicMeshObject::CleanupSharedResources()
{
	if (!m_dwInitRefCount)
		return;

	DWORD ref_count = --m_dwInitRefCount;
	if (!ref_count)
	{
		if (m_pRootSignature)
		{
			m_pRootSignature->Release();
			m_pRootSignature = nullptr;
		}
		if (m_pPipelineState)
		{
			m_pPipelineState->Release();
			m_pPipelineState = nullptr;
		}
	}
}
CBasicMeshObject::~CBasicMeshObject()
{
	Cleanup();
}
```