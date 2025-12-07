---
layout: default
title: "17. CommandListPool"
parent: "(DirectX12 25년 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 사실은 복습에 가까움

---

```cpp
// Linked List로 Pool을 구현
class CCommandListPool
{
	ID3D12Device*	m_pD3DDevice = nullptr;
	D3D12_COMMAND_LIST_TYPE	m_CommnadListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	DWORD	m_dwAllocatedCmdNum = 0;
	DWORD	m_dwAvailableCmdNum = 0;
	DWORD	m_dwTotalCmdNum = 0;
	DWORD	m_dwMaxCmdListNum = 0;
	COMMAND_LIST*	m_pCurCmdList = nullptr;
	SORT_LINK*	m_pAlloatedCmdLinkHead = nullptr;
	SORT_LINK*	m_pAlloatedCmdLinkTail = nullptr;
	SORT_LINK*	m_pAvailableCmdLinkHead = nullptr;
	SORT_LINK*	m_pAvailableCmdLinkTail = nullptr;

	BOOL	AddCmdList();
	COMMAND_LIST*	AllocCmdList();
	void	Cleanup();
public:
	BOOL	Initialize(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, DWORD dwMaxCommandListNum);
	ID3D12GraphicsCommandList*	GetCurrentCommandList();
	void	Close();
	void	CloseAndExecute(ID3D12CommandQueue* pCommandQueue);
	void	Reset();

	DWORD	GetTotalCmdListNum() const { return m_dwTotalCmdNum; }
	DWORD	GetAllocatedCmdListNum() const { return m_dwAllocatedCmdNum; }
	DWORD	GetAvailableCmdListNum() const { return m_dwAvailableCmdNum; }
	ID3D12Device*	INL_GetD3DDevice() { return m_pD3DDevice; }


	CCommandListPool();
	~CCommandListPool();
};
```

```cpp
void CD3D12Renderer::BeginRender()
{
	// command list pool선택 , commalnd list할당
	CCommandListPool*	pCommandListPool = m_ppCommandListPool[m_dwCurContextIndex];
	ID3D12GraphicsCommandList*	pCommandList = pCommandListPool->GetCurrentCommandList();

	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_uiRenderTargetIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRTVHeap->GetCPUDescriptorHandleForHeapStart(), m_uiRenderTargetIndex, m_rtvDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_pDSVHeap->GetCPUDescriptorHandleForHeapStart());

	const float BackColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	pCommandList->ClearRenderTargetView(rtvHandle, BackColor, 0, nullptr);
	pCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// list에 담고 commandqueue를 통해 gpu에 요청한다
	pCommandListPool->CloseAndExecute(m_pCommandQueue);

	Fence();
}
```

```cpp
DWORD CRenderQueue::Process(CCommandListPool* pCommandListPool, ID3D12CommandQueue* pCommandQueue, DWORD dwProcessCountPerCommandList, D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv, const D3D12_VIEWPORT* pViewport, const D3D12_RECT* pScissorRect)
{
	ID3D12Device5* pD3DDevice = m_pRenderer->INL_GetD3DDevice();
	
	ID3D12GraphicsCommandList* ppCommandList[64] = {};
	DWORD	dwCommandListCount = 0;

	ID3D12GraphicsCommandList* pCommandList = nullptr;
	DWORD dwProcessedCount = 0;
	DWORD dwProcessedCountPerCommandList = 0;
	const RENDER_ITEM* pItem = nullptr;
	while (pItem = Dispatch())
	{
        // 여기서 할당
		pCommandList = pCommandListPool->GetCurrentCommandList();

        // list마다 이 Set을 해야함을 기억하자.
		pCommandList->RSSetViewports(1, pViewport);
		pCommandList->RSSetScissorRects(1, pScissorRect);
		pCommandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

		switch (pItem->Type)
		{
			case RENDER_ITEM_TYPE_MESH_OBJ:
				{
					CBasicMeshObject* pMeshObj = (CBasicMeshObject*)pItem->pObjHandle;
					pMeshObj->Draw(pCommandList, &pItem->MeshObjParam.matWorld);
				}
				break;
			case RENDER_ITEM_TYPE_SPRITE:
				{
					CSpriteObject* pSpriteObj = (CSpriteObject*)pItem->pObjHandle;
					TEXTURE_HANDLE* pTexureHandle = (TEXTURE_HANDLE*)pItem->SpriteParam.pTexHandle;
					float Z = pItem->SpriteParam.Z;

					if (pTexureHandle)
					{
						XMFLOAT2 Pos = { (float)pItem->SpriteParam.iPosX, (float)pItem->SpriteParam.iPosY };
						XMFLOAT2 Scale = { pItem->SpriteParam.fScaleX, pItem->SpriteParam.fScaleY };
						
						const RECT*  pRect = nullptr;
						if (pItem->SpriteParam.bUseRect)
						{
							pRect = &pItem->SpriteParam.Rect;
						}

						if (pTexureHandle->pUploadBuffer)
						{
							if (pTexureHandle->bUpdated)
							{
								UpdateTexture(pD3DDevice, pCommandList, pTexureHandle->pTexResource, pTexureHandle->pUploadBuffer);
							}
							else
							{
								int a = 0;
							}
							pTexureHandle->bUpdated = FALSE;
						}
						pSpriteObj->DrawWithTex(pCommandList, &Pos, &Scale, pRect, Z, pTexureHandle);
					}
					else
					{
						CSpriteObject* pSpriteObj = (CSpriteObject*)pItem->pObjHandle;
						XMFLOAT2 Pos = { (float)pItem->SpriteParam.iPosX, (float)pItem->SpriteParam.iPosY };
						XMFLOAT2 Scale = { pItem->SpriteParam.fScaleX, pItem->SpriteParam.fScaleY };

						pSpriteObj->Draw(pCommandList, &Pos, &Scale, Z);

					}
				}
				break;
			default:
				__debugbreak();
		}
		dwProcessedCount++;
		dwProcessedCountPerCommandList++;
		if (dwProcessedCountPerCommandList > dwProcessCountPerCommandList)
		{
			//pCommandListPool->CloseAndExecute(pCommandQueue);
			pCommandListPool->Close();
			ppCommandList[dwCommandListCount] = pCommandList;
			dwCommandListCount++;
			pCommandList = nullptr;
			dwProcessedCountPerCommandList = 0;
		}
	}
	// 남은 렌더링아이템 처리
	if (dwProcessedCountPerCommandList)
	{
		//pCommandListPool->CloseAndExecute(pCommandQueue);
		pCommandListPool->Close();
		ppCommandList[dwCommandListCount] = pCommandList;
		dwCommandListCount++;
		pCommandList = nullptr;
		dwProcessedCountPerCommandList = 0;
	}
	if (dwCommandListCount)
	{
		pCommandQueue->ExecuteCommandLists(dwCommandListCount, (ID3D12CommandList**)ppCommandList);
	}
	m_dwItemCount = 0;
	return dwProcessedCount;
}
```