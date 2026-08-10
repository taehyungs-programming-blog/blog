---
layout: default
title: "13. sprite"
parent: "(DirectX12 25년 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 투명이 있는 텍스처를 sprite라 하고 이 텍스처를 업로드 해보자
* 이걸 왜 할까?
    * text rendering의 베이스가 된다.

---

* sprite를 만드는 곳 부터

```cpp
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance,
					  _In_ LPWSTR    lpCmdLine,
					  _In_ int       nCmdShow)
{
	// ...

	g_pSpriteObj0 = g_pRenderer->CreateSpriteObject(L"sprite_1024x1024.dds", 0, 0, 512, 512);
	g_pSpriteObj1 = g_pRenderer->CreateSpriteObject(L"sprite_1024x1024.dds", 512, 0, 1024, 512);
	g_pSpriteObj2 = g_pRenderer->CreateSpriteObject(L"sprite_1024x1024.dds", 0, 512, 512, 1024);
	g_pSpriteObj3 = g_pRenderer->CreateSpriteObject(L"sprite_1024x1024.dds", 512, 512, 1024, 1024);
```

```cpp
void* CD3D12Renderer::CreateSpriteObject(const WCHAR* wchTexFileName, int PosX, int PosY, int Width, int Height)
{
	CSpriteObject* pSprObj = new CSpriteObject;

	RECT rect;
	rect.left = PosX;
	rect.top = PosY;
	rect.right = Width;
	rect.bottom = Height;
	pSprObj->Initialize(this, wchTexFileName, &rect);

	return (void*)pSprObj;
}
```

```cpp
BOOL CSpriteObject::Initialize(CD3D12Renderer* pRenderer, const WCHAR* wchTexFileName, const RECT* pRect)
{
	m_pRenderer = pRenderer;

	BOOL bResult = (InitCommonResources() != 0);
	if (bResult)
	{
		UINT TexWidth = 1;
		UINT TexHeight = 1;
		m_pTexHandle = (TEXTURE_HANDLE*)m_pRenderer->CreateTextureFromFile(wchTexFileName);
		
        // ...
```

```cpp
BOOL CSpriteObject::InitCommonResources()
{
	if (m_dwInitRefCount)
		goto lb_true;

    // 여기서 오해할 사람이 있음, 오잉? RootSignature를 새로만드다고? -> 새로만드는게 맞음. Pipeline별로 만들고 재활용하는건 CommandList이다.
	InitRootSinagture();
	InitPipelineState();
	InitMesh();

lb_true:
	m_dwInitRefCount++;
	return m_dwInitRefCount;
}
```

* Draw!

```cpp
void CD3D12Renderer::RenderSprite(void* pSprObjHandle, int iPosX, int iPosY, float fScaleX, float fScaleY, float Z)
{
    // CommandList를 넘기는것을 주목.
	ID3D12GraphicsCommandList* pCommandList = m_ppCommandList[m_dwCurContextIndex];

	CSpriteObject* pSpriteObj = (CSpriteObject*)pSprObjHandle;

	XMFLOAT2 Pos = { (float)iPosX, (float)iPosY };
	XMFLOAT2 Scale = { fScaleX, fScaleY };
	pSpriteObj->Draw(pCommandList, &Pos, &Scale, Z);
}
```

```cpp
void CSpriteObject::DrawWithTex(ID3D12GraphicsCommandList* pCommandList, const XMFLOAT2* pPos, const XMFLOAT2* pScale, const RECT* pRect, float Z, TEXTURE_HANDLE* pTexHandle)
{
	// 각각의 draw()작업의 무결성을 보장하려면 draw() 작업마다 다른 영역의 descriptor table(shader visible)과 다른 영역의 CBV를 사용해야 한다.
	// 따라서 draw()할 때마다 CBV는 ConstantBuffer Pool로부터 할당받고, 렌더리용 descriptor table(shader visible)은 descriptor pool로부터 할당 받는다.

	ID3D12Device5* pD3DDeivce = m_pRenderer->INL_GetD3DDevice();
	UINT srvDescriptorSize = m_pRenderer->INL_GetSrvDescriptorSize();
	CDescriptorPool* pDescriptorPool = m_pRenderer->INL_GetDescriptorPool();
	ID3D12DescriptorHeap* pDescriptorHeap = pDescriptorPool->INL_GetDescriptorHeap();
	CSimpleConstantBufferPool* pConstantBufferPool = m_pRenderer->GetConstantBufferPool(CONSTANT_BUFFER_TYPE_SPRITE);
	
    // ...
```

* 복습, CDescriptorPool

```cpp
BOOL CDescriptorPool::Initialize(ID3D12Device5* pD3DDevice, UINT MaxDescriptorCount)
{
    // Heap을 할당하고
	BOOL bResult = FALSE;
	m_pD3DDevice = pD3DDevice;
	
	m_MaxDescriptorCount = MaxDescriptorCount;
	m_srvDescriptorSize = m_pD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// create descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC commonHeapDesc = {};
	commonHeapDesc.NumDescriptors = m_MaxDescriptorCount;
	commonHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	commonHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	if (FAILED(m_pD3DDevice->CreateDescriptorHeap(&commonHeapDesc, IID_PPV_ARGS(&m_pDescritorHeap))))
	{
		__debugbreak();
		goto lb_return;
	}
	m_cpuDescriptorHandle = m_pDescritorHeap->GetCPUDescriptorHandleForHeapStart();
	m_gpuDescriptorHandle = m_pDescritorHeap->GetGPUDescriptorHandleForHeapStart();
	bResult = TRUE;
lb_return:
	return bResult;

}
BOOL CDescriptorPool::AllocDescriptorTable(D3D12_CPU_DESCRIPTOR_HANDLE* pOutCPUDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE* pOutGPUDescriptor, UINT DescriptorCount)
{
    // offset으로 주소를 넘긴다
	BOOL bResult = FALSE;
	if (m_AllocatedDescriptorCount + DescriptorCount > m_MaxDescriptorCount)
	{
		goto lb_return;
	}
	UINT offset = m_AllocatedDescriptorCount + DescriptorCount;

	*pOutCPUDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cpuDescriptorHandle, m_AllocatedDescriptorCount, m_srvDescriptorSize);
	*pOutGPUDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_gpuDescriptorHandle, m_AllocatedDescriptorCount, m_srvDescriptorSize);
	m_AllocatedDescriptorCount += DescriptorCount;
	bResult = TRUE;
lb_return:
	return bResult;
}
```

```cpp
void CSpriteObject::DrawWithTex(ID3D12GraphicsCommandList* pCommandList, const XMFLOAT2* pPos, const XMFLOAT2* pScale, const RECT* pRect, float Z, TEXTURE_HANDLE* pTexHandle)
{

    // ...

    // table에 복사 하고 마무리.
    CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(cpuDescriptorTable, SPRITE_DESCRIPTOR_INDEX_CBV, srvDescriptorSize);
    pD3DDeivce->CopyDescriptorsSimple(1, cbvDest, pCB->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    if (srv.ptr)
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE srvDest(cpuDescriptorTable, SPRITE_DESCRIPTOR_INDEX_TEX, srvDescriptorSize);
        pD3DDeivce->CopyDescriptorsSimple(1, srvDest, srv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
```

* 근데 이게 투명도 처리가 되나?
    * 사실 안됨 ㅋㅋ
    * 이렇게 수정해야합니다.

```cpp
// Describe and create the graphics pipeline state object (PSO).
D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
psoDesc.pRootSignature = m_pRootSignature;
psoDesc.VS = CD3DX12_SHADER_BYTECODE(pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize());
psoDesc.PS = CD3DX12_SHADER_BYTECODE(pPixelShader->GetBufferPointer(), pPixelShader->GetBufferSize());
psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

// blend state를 추가
psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
```

```cpp
float4 PSMain(PSInput input) : SV_TARGET
{
    float4 texColor = texDiffuse.Sample(samplerDiffuse, input.TexCoord);
    texColor.a *= g_Alpha; // pixel shader 알파블랜드 추가
    return texColor * input.color;
}

```

```cpp

```