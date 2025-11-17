---
layout: default
title: "05. Texture 써보기"
parent: "(DirectX12 25년 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 사전 복습) Descriptor Heap과 Descriptor Table

* Descriptor란?
    * Descriptor는 GPU 리소스(텍스처, 버퍼, 샘플러 등)를 설명하는 작은 데이터 블록
    * GPU가 리소스에 접근하는 방법을 정의

### Descriptor Heap

* Descriptor Heap
    * descriptor들을 저장하는 GPU 메모리의 연속된 공간
    * 마치 descriptor들의 배열
* 종류?
    * D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: Constant Buffer View, Shader Resource View, Unordered Access View
    * D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: 샘플러
    * D3D12_DESCRIPTOR_HEAP_TYPE_RTV: Render Target View
    * D3D12_DESCRIPTOR_HEAP_TYPE_DSV: Depth Stencil View

```cpp
// Descriptor Heap 생성
D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
heapDesc.NumDescriptors = 1000;
heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

ID3D12DescriptorHeap* descriptorHeap;
device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap));
```

### Descriptor Table

* Descriptor Table
    * Descriptor Heap 내의 descriptor들의 연속된 범위를 가리키는 포인터
    * Root Signature에서 사용

```cpp
// Descriptor Table 정의
D3D12_DESCRIPTOR_RANGE ranges[2];

// Range 1: SRV 5개
ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
ranges[0].NumDescriptors = 5;
ranges[0].BaseShaderRegister = 0; // t0-t4
ranges[0].RegisterSpace = 0;
ranges[0].OffsetInDescriptorsFromTableStart = 0;

// Range 2: CBV 2개
ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
ranges[1].NumDescriptors = 2;
ranges[1].BaseShaderRegister = 0; // b0-b1
ranges[1].RegisterSpace = 0;
ranges[1].OffsetInDescriptorsFromTableStart = 5;

// Root Parameter로 Descriptor Table 설정
D3D12_ROOT_PARAMETER rootParam;
rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
rootParam.DescriptorTable.NumDescriptorRanges = 2;
rootParam.DescriptorTable.pDescriptorRanges = ranges;
rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
```

```cpp
// Command List에 Descriptor Table 바인딩
commandList->SetGraphicsRootDescriptorTable(
    0, // Root parameter 인덱스
    descriptorHeap->GetGPUDescriptorHandleForHeapStart() // Table 시작 위치
);
```

---

## Texture를 써보자!

* Texture를 쓰기 위해서, 이런 쉐이더를 쓸껀데 어떻게 사용되는지 보는게 핵심이다.

```cpp
Texture2D texDiffuse : register(t0);
SamplerState samplerDiffuse	: register(s0);

struct VSInput
{
    float4 Pos : POSITION;
    float4 color : COLOR;
    float2 TexCoord : TEXCOORD0;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 TexCoord : TEXCOORD0;
};

PSInput VSMain(VSInput input)
{
    PSInput result = (PSInput)0;

    result.position = input.Pos;
    result.TexCoord = input.TexCoord;
    result.color = input.color;
    
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4	texColor = texDiffuse.Sample(samplerDiffuse, input.TexCoord);
    return texColor * input.color;
}
```

```cpp
BOOL CBasicMeshObject::InitRootSinagture()
{
	ID3D12Device5* pD3DDeivce = m_pRenderer->INL_GetD3DDevice();
	ID3DBlob* pSignature = nullptr;
	ID3DBlob* pError = nullptr;

    /*
    rootsignature를 만들며 texture를 위한 descriptor하나와
    sampler 하나를 만들어 둔다.
    */
	CD3DX12_DESCRIPTOR_RANGE ranges[1] = {};
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	// t0 : texture
	
	CD3DX12_ROOT_PARAMETER rootParameters[1] = {};
	rootParameters[0].InitAsDescriptorTable(_countof(ranges), ranges, D3D12_SHADER_VISIBILITY_ALL);

	// default sampler
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	SetDefaultSamplerDesc(&sampler, 0);
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;

	// Allow input layout and deny uneccessary access to certain pipeline stages.
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	// Create an root signature.
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	//rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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
```

* Texture 생성

```cpp
BOOL CD3D12ResourceManager::CreateTexture(ID3D12Resource** ppOutResource, UINT Width, UINT Height, DXGI_FORMAT format, const BYTE* pInitImage)
{
	ID3D12Resource*	pTexResource = nullptr;
	ID3D12Resource*	pUploadBuffer = nullptr;

	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.MipLevels = 1;
	textureDesc.Format = format;	// ex) DXGI_FORMAT_R8G8B8A8_UNORM, etc...
	textureDesc.Width = Width;
	textureDesc.Height = Height;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    // GPU 메모리(DEFAULT HEAP)에 텍스처 생성
	if (FAILED(m_pD3DDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&pTexResource))))
	{
		__debugbreak();
	}

	if (pInitImage)
	{
		D3D12_RESOURCE_DESC Desc = pTexResource->GetDesc();
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint;
		UINT	Rows = 0;
		UINT64	RowSize = 0;
		UINT64	TotalBytes = 0;

        // 복사에 필요한 메모리 레이아웃 정보 가져오기
		m_pD3DDevice->GetCopyableFootprints(&Desc, 0, 1, 0, &Footprint, &Rows, &RowSize, &TotalBytes);

        // CPU 메모리에 데이터 복사
		BYTE*	pMappedPtr = nullptr;
		CD3DX12_RANGE readRange(0, 0);

		UINT64 uploadBufferSize = GetRequiredIntermediateSize(pTexResource, 0, 1);

        // 업로드 버퍼 생성 (CPU→GPU 전송용)
		if (FAILED(m_pD3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // CPU→GPU 업로드용 메모리
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,                // // GPU가 읽을 수 있는 상태
			nullptr,
			IID_PPV_ARGS(&pUploadBuffer))))
		{
			__debugbreak();
		}

		HRESULT hr = pUploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pMappedPtr));
		if (FAILED(hr))
			__debugbreak();

		const BYTE* pSrc = pInitImage;
		BYTE* pDest = pMappedPtr;
		for (UINT y = 0; y < Height; y++)
		{
			memcpy(pDest, pSrc, Width * 4);
			pSrc += (Width * 4);
			pDest += Footprint.Footprint.RowPitch;			
		}

        /*
        소스 이미지 (pInitImage):
        [RGBA][RGBA][RGBA]... (Width * 4 바이트)
        [RGBA][RGBA][RGBA]... (Width * 4 바이트)

        업로드 버퍼 (정렬 필요):
        [RGBA][RGBA][RGBA]...[패딩] (RowPitch 바이트)
        [RGBA][RGBA][RGBA]...[패딩] (RowPitch 바이트)
        */

		// Unmap
		pUploadBuffer->Unmap(0, nullptr);

		UpdateTextureForWrite(pTexResource, pUploadBuffer);

		pUploadBuffer->Release();
		pUploadBuffer = nullptr;
		
	}
	*ppOutResource = pTexResource;

	return TRUE;
}
```

```cpp
BOOL CBasicMeshObject::CreateDescriptorTable()
{

	BOOL bResult = FALSE;
	ID3D12Device5* pD3DDeivce = m_pRenderer->INL_GetD3DDevice();

	
	m_srvDescriptorSize = pD3DDeivce->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// create descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC commonHeapDesc = {};
	commonHeapDesc.NumDescriptors = DESCRIPTOR_COUNT_FOR_DRAW;
	commonHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	commonHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	if (FAILED(pD3DDeivce->CreateDescriptorHeap(&commonHeapDesc, IID_PPV_ARGS(&m_pDescritorHeap))))
	{
		__debugbreak();
		goto lb_return;
	}
	bResult = TRUE;
lb_return:
	return bResult;

}
```

---

## 정리해 보자면..

```
1. 텍스처 리소스 생성 (GPU 메모리)
   ↓
2. Descriptor (SRV) 생성
   ↓
3. Descriptor Heap에 배치
   ↓
4. Root Signature 정의 (셰이더 입력 레이아웃)
   ↓
5. Pipeline State Object (PSO) 생성
   ↓
6. Command List에서 바인딩
   ↓
7. Draw Call 실행
   ↓
8. 셰이더에서 텍스처 샘플링
```

### 1단계: 텍스처 리소스 생성

```cpp
// 이미 CreateTexture 함수로 생성됨
ID3D12Resource* pTexture = nullptr;
CreateTexture(&pTexture, 256, 256, DXGI_FORMAT_R8G8B8A8_UNORM, imageData);

// 현재 상태: GPU 메모리에 텍스처 데이터 존재
// 하지만 GPU는 아직 이것을 "어떻게" 사용할지 모름
```

### 2단계: Shader Resource View (SRV) 생성

```cpp
// SRV Descriptor Heap 생성 (여러 텍스처를 위한 공간)
D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
srvHeapDesc.NumDescriptors = 100;  // 최대 100개의 텍스처/버퍼
srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  // 셰이더에서 접근 가능

ID3D12DescriptorHeap* pSrvHeap = nullptr;
m_pD3DDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&pSrvHeap));

// Descriptor 크기 계산 (GPU마다 다름)
UINT descriptorSize = m_pD3DDevice->GetDescriptorHandleIncrementSize(
    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

// Descriptor Heap의 시작 주소 얻기
D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = pSrvHeap->GetCPUDescriptorHandleForHeapStart();

// SRV 생성 - 텍스처를 "어떻게" 읽을지 정의
D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
srvDesc.Texture2D.MipLevels = 1;
srvDesc.Texture2D.MostDetailedMip = 0;

// CPU에서 Descriptor 작성
m_pD3DDevice->CreateShaderResourceView(
    pTexture,   // 실제 텍스처 리소스
    &srvDesc,   // 읽는 방법
    srvHandle   // Descriptor Heap의 위치
);
```

```
Descriptor Heap (GPU 메모리):
[Index 0: Texture SRV] ← 방금 생성한 SRV
[Index 1: 비어있음]
[Index 2: 비어있음]
...
[Index 99: 비어있음]

각 Descriptor는 다음 정보를 포함:
- 텍스처 리소스의 GPU 주소
- 포맷 정보
- 크기, 밉맵 레벨 등
```

### 3단계: Root Signature 정의

```cpp
// Descriptor Table 범위 정의
D3D12_DESCRIPTOR_RANGE ranges[2];

// Range 0: 텍스처 (SRV)
ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
ranges[0].NumDescriptors = 1;           // 텍스처 1개
ranges[0].BaseShaderRegister = 0;       // register(t0)에 바인딩
ranges[0].RegisterSpace = 0;            // space0
ranges[0].OffsetInDescriptorsFromTableStart = 0;

// Range 1: 샘플러
ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
ranges[1].NumDescriptors = 1;
ranges[1].BaseShaderRegister = 0;       // register(s0)에 바인딩
ranges[1].RegisterSpace = 0;
ranges[1].OffsetInDescriptorsFromTableStart = 0;

// Root Parameter 정의
D3D12_ROOT_PARAMETER rootParams[2];

// Parameter 0: SRV Descriptor Table
rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
rootParams[0].DescriptorTable.NumDescriptorRanges = 1;
rootParams[0].DescriptorTable.pDescriptorRanges = &ranges[0];
rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;  // Pixel Shader에서만 사용

// Parameter 1: Sampler Descriptor Table
rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
rootParams[1].DescriptorTable.NumDescriptorRanges = 1;
rootParams[1].DescriptorTable.pDescriptorRanges = &ranges[1];
rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

// Root Signature 생성
D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
rootSigDesc.NumParameters = 2;
rootSigDesc.pParameters = rootParams;
rootSigDesc.NumStaticSamplers = 0;
rootSigDesc.pStaticSamplers = nullptr;
rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

ID3DBlob* pSignatureBlob = nullptr;
ID3DBlob* pErrorBlob = nullptr;
D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, 
                            &pSignatureBlob, &pErrorBlob);

ID3D12RootSignature* pRootSignature = nullptr;
m_pD3DDevice->CreateRootSignature(
    0, 
    pSignatureBlob->GetBufferPointer(), 
    pSignatureBlob->GetBufferSize(), 
    IID_PPV_ARGS(&pRootSignature)
);
```

> 4, 5는 생략

### 6단계: 렌더링 시 바인딩 (Command List)

```cpp
// Command List 기록 시작
pCommandList->Reset(pCommandAllocator, pPSO);

// Root Signature 설정
pCommandList->SetGraphicsRootSignature(pRootSignature);

// Descriptor Heap들을 Command List에 설정
ID3D12DescriptorHeap* ppHeaps[] = { pSrvHeap, pSamplerHeap };
pCommandList->SetDescriptorHeaps(2, ppHeaps);

// Root Parameter에 Descriptor Table 바인딩
// Parameter 0: 텍스처 SRV
D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = pSrvHeap->GetGPUDescriptorHandleForHeapStart();
pCommandList->SetGraphicsRootDescriptorTable(0, srvGpuHandle);

// Parameter 1: 샘플러
D3D12_GPU_DESCRIPTOR_HANDLE samplerGpuHandle = pSamplerHeap->GetGPUDescriptorHandleForHeapStart();
pCommandList->SetGraphicsRootDescriptorTable(1, samplerGpuHandle);

// 렌더 타겟 설정
pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

// Viewport, Scissor 설정
pCommandList->RSSetViewports(1, &viewport);
pCommandList->RSSetScissorRects(1, &scissorRect);

// 버텍스 버퍼 설정
pCommandList->IASetVertexBuffers(0, 1, &vertexBufferView);
pCommandList->IASetIndexBuffer(&indexBufferView);
pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

// 드로우 콜!
pCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

// Command List 종료
pCommandList->Close();

// Command Queue에 제출
m_pCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList**)&pCommandList);
```

```
GPU의 시각:

Root Signature (계약서):
  Root Param[0] → register(t0) ← SRV Descriptor Table 필요
  Root Param[1] → register(s0) ← Sampler Descriptor Table 필요

SetGraphicsRootDescriptorTable(0, srvGpuHandle):
  "Root Param[0]는 이 GPU 주소의 Descriptor를 사용해!"
  
SetGraphicsRootDescriptorTable(1, samplerGpuHandle):
  "Root Param[1]는 이 GPU 주소의 Descriptor를 사용해!"

Draw 호출 시:
  GPU는 Root Signature를 보고
  → Root Param[0]의 GPU 주소에서 SRV Descriptor 읽음
  → Descriptor에서 실제 텍스처 리소스 주소 얻음
  → 셰이더에 전달
```

### 7단계: 셰이더 코드에서 사용

```
// Vertex Shader
struct VSInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = float4(input.position, 1.0f);
    output.texCoord = input.texCoord;
    return output;
}

// Pixel Shader
Texture2D    g_Texture : register(t0);  // ← Root Parameter[0]에서 옴
SamplerState g_Sampler : register(s0);  // ← Root Parameter[1]에서 옴

float4 PSMain(VSOutput input) : SV_TARGET
{
    // 텍스처 샘플링!
    float4 color = g_Texture.Sample(g_Sampler, input.texCoord);
    return color;
}
```