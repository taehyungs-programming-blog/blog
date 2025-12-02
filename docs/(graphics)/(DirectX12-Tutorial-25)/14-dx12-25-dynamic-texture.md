---
layout: default
title: "14. texture를 갱신해 보자"
parent: "(DirectX12 25년 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 그리 어렵지 않기에 코드로 설명합니다.

## Create

```cpp
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance,
					  _In_ LPWSTR    lpCmdLine,
					  _In_ int       nCmdShow)
{
    // ...

    // void* g_pDynamicTexHandle = nullptr;
    g_pDynamicTexHandle = g_pRenderer->CreateDynamicTexture(g_ImageWidth, g_ImageHeight);
```

```cpp
void* CD3D12Renderer::CreateDynamicTexture(UINT TexWidth, UINT TexHeight)
{
	TEXTURE_HANDLE* pTexHandle = nullptr;

	ID3D12Resource* pTexResource = nullptr;
	ID3D12Resource* pUploadBuffer = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE srv = {};


	DXGI_FORMAT TexFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (m_pResourceManager->CreateTexturePair(&pTexResource, &pUploadBuffer, TexWidth, TexHeight, TexFormat))
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = TexFormat;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		if (m_pSingleDescriptorAllocator->AllocDescriptorHandle(&srv))
		{
			m_pD3DDevice->CreateShaderResourceView(pTexResource, &SRVDesc, srv);

			pTexHandle = AllocTextureHandle();
			pTexHandle->pTexResource = pTexResource;
			pTexHandle->pUploadBuffer = pUploadBuffer;
			pTexHandle->srv = srv;
		}
		else
		{
			pTexResource->Release();
			pTexResource = nullptr;

			pUploadBuffer->Release();
			pUploadBuffer = nullptr;
		}
	}

	return pTexHandle;
}
```

```cpp
BOOL CD3D12ResourceManager::CreateTexturePair(ID3D12Resource** ppOutResource, ID3D12Resource** ppOutUploadBuffer, UINT Width, UINT Height, DXGI_FORMAT format)
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

	UINT64 uploadBufferSize = GetRequiredIntermediateSize(pTexResource, 0, 1);

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
	*ppOutResource = pTexResource;
	*ppOutUploadBuffer = pUploadBuffer;

	return TRUE;
}
```

---

## Draw

```cpp
void CD3D12Renderer::RenderSpriteWithTex(void* pSprObjHandle, int iPosX, int iPosY, float fScaleX, float fScaleY, const RECT* pRect, float Z, void* pTexHandle)
{
	ID3D12GraphicsCommandList* pCommandList = m_ppCommandList[m_dwCurContextIndex];
	TEXTURE_HANDLE* pTexureHandle = (TEXTURE_HANDLE*)pTexHandle;

	CSpriteObject* pSpriteObj = (CSpriteObject*)pSprObjHandle;

	XMFLOAT2 Pos = { (float)iPosX, (float)iPosY };
	XMFLOAT2 Scale = { fScaleX, fScaleY };

	if (pTexureHandle->pUploadBuffer)
	{
		if (pTexureHandle->bUpdated)
		{
			UpdateTexture(m_pD3DDevice, pCommandList, pTexureHandle->pTexResource, pTexureHandle->pUploadBuffer);
		}
		else
		{
			int a = 0;
		}
		pTexureHandle->bUpdated = FALSE;
	}
	pSpriteObj->DrawWithTex(pCommandList, &Pos, &Scale, pRect, Z, pTexureHandle);
}
```

```cpp
// 여기서 Update를 한 번 받아야 한다
void CD3D12Renderer::UpdateTextureWithImage(void* pTexHandle, const BYTE* pSrcBits, UINT SrcWidth, UINT SrcHeight)
{	
	TEXTURE_HANDLE* pTextureHandle = (TEXTURE_HANDLE*)pTexHandle;
	ID3D12Resource* pDestTexResource = pTextureHandle->pTexResource;
	ID3D12Resource* pUploadBuffer = pTextureHandle->pUploadBuffer;

	D3D12_RESOURCE_DESC Desc = pDestTexResource->GetDesc();
	if (SrcWidth > Desc.Width)
	{
		__debugbreak();
	}
	if (SrcHeight > Desc.Height)
	{
		__debugbreak();
	}
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint;
	UINT	Rows = 0;
	UINT64	RowSize = 0;
	UINT64	TotalBytes = 0;

	m_pD3DDevice->GetCopyableFootprints(&Desc, 0, 1, 0, &Footprint, &Rows, &RowSize, &TotalBytes);

	BYTE*	pMappedPtr = nullptr;
	CD3DX12_RANGE readRange(0, 0);

	HRESULT hr = pUploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pMappedPtr));
	if (FAILED(hr))
		__debugbreak();

	const BYTE* pSrc = pSrcBits;
	BYTE* pDest = pMappedPtr;
	for (UINT y = 0; y < SrcHeight; y++)
	{
		memcpy(pDest, pSrc, SrcWidth * 4);
		pSrc += (SrcWidth * 4);
		pDest += Footprint.Footprint.RowPitch;			
	}
	// Unmap
	pUploadBuffer->Unmap(0, nullptr);

	pTextureHandle->bUpdated = TRUE;
}
```

```cpp
void UpdateTexture(ID3D12Device* pD3DDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pDestTexResource, ID3D12Resource* pSrcTexResource)
{
	const DWORD MAX_SUB_RESOURCE_NUM = 32;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint[MAX_SUB_RESOURCE_NUM] = {};
	UINT	Rows[MAX_SUB_RESOURCE_NUM] = {};
	UINT64	RowSize[MAX_SUB_RESOURCE_NUM] = {};
	UINT64	TotalBytes = 0;

	D3D12_RESOURCE_DESC Desc = pDestTexResource->GetDesc();
	if (Desc.MipLevels > (UINT)_countof(Footprint))
		__debugbreak();

	pD3DDevice->GetCopyableFootprints(&Desc, 0, Desc.MipLevels, 0, Footprint, Rows, RowSize, &TotalBytes);

	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pDestTexResource, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST));
	for (DWORD i = 0; i < Desc.MipLevels; i++)
	{

		D3D12_TEXTURE_COPY_LOCATION	destLocation = {};
		destLocation.PlacedFootprint = Footprint[i];
		destLocation.pResource = pDestTexResource;
		destLocation.SubresourceIndex = i;
		destLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

		D3D12_TEXTURE_COPY_LOCATION	srcLocation = {};
		srcLocation.PlacedFootprint = Footprint[i];
		srcLocation.pResource = pSrcTexResource;
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

		pCommandList->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);
	}
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pDestTexResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));

}
```

```cpp
void CSpriteObject::DrawWithTex(ID3D12GraphicsCommandList* pCommandList, const XMFLOAT2* pPos, const XMFLOAT2* pScale, const RECT* pRect, float Z, TEXTURE_HANDLE* pTexHandle)
{
	ID3D12Device5* pD3DDeivce = m_pRenderer->INL_GetD3DDevice();
	UINT srvDescriptorSize = m_pRenderer->INL_GetSrvDescriptorSize();
	CDescriptorPool* pDescriptorPool = m_pRenderer->INL_GetDescriptorPool();
	ID3D12DescriptorHeap* pDescriptorHeap = pDescriptorPool->INL_GetDescriptorHeap();
	CSimpleConstantBufferPool* pConstantBufferPool = m_pRenderer->GetConstantBufferPool(CONSTANT_BUFFER_TYPE_SPRITE);
	
	UINT TexWidth = 0;
	UINT TexHeight = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE srv = {};
	if (pTexHandle)
	{
		D3D12_RESOURCE_DESC desc = pTexHandle->pTexResource->GetDesc();
		TexWidth = desc.Width;
		TexHeight = desc.Height;
		srv = pTexHandle->srv;
	}

	RECT rect;
	if (!pRect)
	{
		rect.left = 0;
		rect.top = 0;
		rect.right = TexWidth;
		rect.bottom = TexHeight;
		pRect = &rect;
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptorTable = {};
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptorTable = {};

	if (!pDescriptorPool->AllocDescriptorTable(&cpuDescriptorTable, &gpuDescriptorTable, DESCRIPTOR_COUNT_FOR_DRAW))
	{
		__debugbreak();
	}

	CB_CONTAINER* pCB = pConstantBufferPool->Alloc();
	if (!pCB)
	{
		__debugbreak();
	}
	CONSTANT_BUFFER_SPRITE* pConstantBufferSprite = (CONSTANT_BUFFER_SPRITE*)pCB->pSystemMemAddr;

	// constant buffer의 내용을 설정
	pConstantBufferSprite->ScreenRes.x = (float)m_pRenderer->INL_GetScreenWidth();
	pConstantBufferSprite->ScreenRes.y = (float)m_pRenderer->INL_GetScreenHeigt();
	pConstantBufferSprite->Pos = *pPos;
	pConstantBufferSprite->Scale = *pScale;
	pConstantBufferSprite->TexSize.x = (float)TexWidth;
	pConstantBufferSprite->TexSize.y = (float)TexHeight;
	pConstantBufferSprite->TexSampePos.x = (float)pRect->left;
	pConstantBufferSprite->TexSampePos.y = (float)pRect->top;
	pConstantBufferSprite->TexSampleSize.x = (float)(pRect->right - pRect->left);
	pConstantBufferSprite->TexSampleSize.y = (float)(pRect->bottom - pRect->top);
	pConstantBufferSprite->Z = Z;
	pConstantBufferSprite->Alpha = 1.0f;

	
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);
	pCommandList->SetDescriptorHeaps(1, &pDescriptorHeap);


	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(cpuDescriptorTable, SPRITE_DESCRIPTOR_INDEX_CBV, srvDescriptorSize);
	pD3DDeivce->CopyDescriptorsSimple(1, cbvDest, pCB->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	if (srv.ptr)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvDest(cpuDescriptorTable, SPRITE_DESCRIPTOR_INDEX_TEX, srvDescriptorSize);
		pD3DDeivce->CopyDescriptorsSimple(1, srvDest, srv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	pCommandList->SetGraphicsRootDescriptorTable(0, gpuDescriptorTable);

	pCommandList->SetPipelineState(m_pPipelineState);
	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	pCommandList->IASetIndexBuffer(&m_IndexBufferView);
	pCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

}
```