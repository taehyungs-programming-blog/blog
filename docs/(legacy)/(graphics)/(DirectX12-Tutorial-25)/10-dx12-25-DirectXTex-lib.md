---
layout: default
title: "10. DirectXTex Lib 사용법"
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

## 준비비

* DXGI에 있으려나?
    * ㅋㅋ DirectX를 우습게 봤군.. 이건 지원하지 않는다
    * [DirectXTex Git](https://github.com/microsoft/DirectXTex) 에서 받도록 하자

---

## DirectXTex

### 우선 기존 Texture를 쓰는방법을 다시 복습해보자

* DirectX 12에서 텍스처를 GPU에 업로드하는 과정은 다음과 같은 단계를 갖는다

#### 1. GPU 텍스처 리소스 생성

```cpp
// DDS 파일에서 텍스처 로드 (DirectXTex 사용)
std::unique_ptr<uint8_t[]> ddsData;
std::vector<D3D12_SUBRESOURCE_DATA> subresouceData;
LoadDDSTextureFromFile(m_pD3DDevice, wchFileName, &pTexResource, ddsData, subresouceData);
```

#### 2. Upload Buffer 생성

```cpp
UINT64 uploadBufferSize = GetRequiredIntermediateSize(pTexResource, 0, subresoucesize);

m_pD3DDevice->CreateCommittedResource(
    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
    D3D12_HEAP_FLAG_NONE,
    &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
    D3D12_RESOURCE_STATE_GENERIC_READ,
    nullptr,
    IID_PPV_ARGS(&pUploadBuffer));
```

#### 3. Resource Barrier - Copy Dest 상태로 전환

```cpp
m_pCommandList->ResourceBarrier(1, 
    &CD3DX12_RESOURCE_BARRIER::Transition(
        pTexResource, 
        D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, 
        D3D12_RESOURCE_STATE_COPY_DEST));
```

#### 4. Upload Buffer에서 GPU Buffer로 복사

```cpp
UpdateSubresources(m_pCommandList, pTexResource, pUploadBuffer, 
    0, 0, subresoucesize, &subresouceData[0]);
```

#### 5. Resource Barrier - Shader Resource 상태로 전환

```cpp
m_pCommandList->ResourceBarrier(1, 
    &CD3DX12_RESOURCE_BARRIER::Transition(
        pTexResource, 
        D3D12_RESOURCE_STATE_COPY_DEST, 
        D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));
```

#### 6. Command List Close & Execute

```cpp
m_pCommandList->Close();

ID3D12CommandList* ppCommandLists[] = { m_pCommandList };
m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

Fence();
WaitForFenceValue();
```

#### 7. Upload Buffer 정리

```cpp
if (pUploadBuffer)
{
    pUploadBuffer->Release();
    pUploadBuffer = nullptr;
}
```

---

### 실제로 어떻게 구현하나 보자

```cpp
void* CD3D12Renderer::CreateTextureFromFile(const WCHAR* wchFileName)
{
	TEXTURE_HANDLE* pTexHandle = nullptr;

	ID3D12Resource* pTexResource = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE srv = {};

	DXGI_FORMAT TexFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D12_RESOURCE_DESC	desc = {};
	if (m_pResourceManager->CreateTextureFromFile(&pTexResource, &desc, wchFileName))
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = desc.Format;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = desc.MipLevels;

		if (m_pSingleDescriptorAllocator->AllocDescriptorHandle(&srv))
		{
			m_pD3DDevice->CreateShaderResourceView(pTexResource, &SRVDesc, srv);

			pTexHandle = new TEXTURE_HANDLE;
			pTexHandle->pTexResource = pTexResource;
			pTexHandle->srv = srv;
		}
		else
		{
			pTexResource->Release();
			pTexResource = nullptr;
		}
	}

	return pTexHandle;
}
```

```cpp
BOOL CD3D12ResourceManager::CreateTextureFromFile(ID3D12Resource** ppOutResource, D3D12_RESOURCE_DESC* pOutDesc, const WCHAR* wchFileName)
{
	BOOL bResult = FALSE;

	ID3D12Resource*	pTexResource = nullptr;
	ID3D12Resource*	pUploadBuffer = nullptr;

	D3D12_RESOURCE_DESC textureDesc = {};

	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> subresouceData;
	if (FAILED(LoadDDSTextureFromFile(m_pD3DDevice, wchFileName, &pTexResource, ddsData, subresouceData)))
	{
		goto lb_return;
	}
	textureDesc = pTexResource->GetDesc();
	UINT subresoucesize = (UINT)subresouceData.size();	
	UINT64 uploadBufferSize = GetRequiredIntermediateSize(pTexResource, 0, subresoucesize);

	// Create the GPU upload buffer.
	if (FAILED(m_pD3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pUploadBuffer))))
		{
			__debugbreak();
		}

	if (FAILED(m_pCommandAllocator->Reset()))
		__debugbreak();

	if (FAILED(m_pCommandList->Reset(m_pCommandAllocator, nullptr)))
		__debugbreak();

	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pTexResource, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources(m_pCommandList, pTexResource, pUploadBuffer,	0, 0, subresoucesize, &subresouceData[0]);
	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pTexResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));

	m_pCommandList->Close();

	ID3D12CommandList* ppCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	Fence();
	WaitForFenceValue();

	if (pUploadBuffer)
	{
		pUploadBuffer->Release();
		pUploadBuffer = nullptr;
	}
	*ppOutResource = pTexResource;
	*pOutDesc = textureDesc;
	bResult = TRUE;
lb_return:
	return bResult;
}
```

---

### Render는 어떻게 이루어질까?

```cpp
void RunGame()
{
	// ...

    // 여기서 Texture를를 넣어준다
	g_pRenderer->RenderMeshObject(g_pMeshObj, &g_matWorld1, g_pTexHandle1);
```

```cpp
void CD3D12Renderer::RenderMeshObject(void* pMeshObjHandle, const XMMATRIX* pMatWorld, void* pTexHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE srv = {};
	CBasicMeshObject* pMeshObj = (CBasicMeshObject*)pMeshObjHandle;
	if (pTexHandle)
	{
		srv = ((TEXTURE_HANDLE*)pTexHandle)->srv;
	}
	pMeshObj->Draw(m_pCommandList, pMatWorld, srv);
}
```

```cpp
void CBasicMeshObject::Draw(ID3D12GraphicsCommandList* pCommandList, const XMMATRIX* pMatWorld, D3D12_CPU_DESCRIPTOR_HANDLE srv)
{
	// 각각의 draw()작업의 무결성을 보장하려면 draw() 작업마다 다른 영역의 descriptor table(shader visible)과 다른 영역의 CBV를 사용해야 한다.
	// 따라서 draw()할 때마다 CBV는 ConstantBuffer Pool로부터 할당받고, 렌더리용 descriptor table(shader visible)은 descriptor pool로부터 할당 받는다.

	ID3D12Device5* pD3DDeivce = m_pRenderer->INL_GetD3DDevice();
	UINT srvDescriptorSize = m_pRenderer->INL_GetSrvDescriptorSize();
	CDescriptorPool* pDescriptorPool = m_pRenderer->INL_GetDescriptorPool();
	ID3D12DescriptorHeap* pDescriptorHeap = pDescriptorPool->INL_GetDescriptorHeap();
	CSimpleConstantBufferPool* pConstantBufferPool = m_pRenderer->INL_GetConstantBufferPool();
	

	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptorTable = {};
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptorTable = {};

	if (!pDescriptorPool->AllocDescriptorTable(&cpuDescriptorTable, &gpuDescriptorTable, DESCRIPTOR_COUNT_FOR_DRAW))
	{
		__debugbreak();
	}

	// 각각의 draw()에 대해 독립적인 constant buffer(내부적으로는 같은 resource의 다른 영역)를 사용한다.
	CB_CONTAINER* pCB = pConstantBufferPool->Alloc();
	if (!pCB)
	{
		__debugbreak();
	}
	CONSTANT_BUFFER_DEFAULT* pConstantBufferDefault = (CONSTANT_BUFFER_DEFAULT*)pCB->pSystemMemAddr;

	// constant buffer의 내용을 설정
	// view/proj matrix
	m_pRenderer->GetViewProjMatrix(&pConstantBufferDefault->matView, &pConstantBufferDefault->matProj);
	
	// world matrix
	pConstantBufferDefault->matWorld = XMMatrixTranspose(*pMatWorld);

	// set RootSignature
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	pCommandList->SetDescriptorHeaps(1, &pDescriptorHeap);

	// 이번에 사용할 constant buffer의 descriptor를 렌더링용(shader visible) descriptor table에 카피
	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(cpuDescriptorTable, BASIC_MESH_DESCRIPTOR_INDEX_CBV, srvDescriptorSize);
	pD3DDeivce->CopyDescriptorsSimple(1, cbvDest, pCB->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);	// cpu측 코드에서는 cpu descriptor handle에만 write가능
	
	// srv(texture)의 descriptor를 렌더링용(shader visible) descriptor table에 카피
	if (srv.ptr)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvDest(cpuDescriptorTable, BASIC_MESH_DESCRIPTOR_INDEX_TEX, srvDescriptorSize);
		pD3DDeivce->CopyDescriptorsSimple(1, srvDest, srv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);	// cpu측 코드에서는 cpu descriptor handle에만 write가능
	}
	
	pCommandList->SetGraphicsRootDescriptorTable(0, gpuDescriptorTable);

	pCommandList->SetPipelineState(m_pPipelineState);
	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	pCommandList->IASetIndexBuffer(&m_IndexBufferView);
	pCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	

}
```