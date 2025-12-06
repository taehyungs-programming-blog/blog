---
layout: default
title: "17. render queue"
parent: "(DirectX12 25년 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 이게 왜 필요할까?

### 1. Render를 하나로 모으자(Render Queue!)

```cpp
// 문제상황황
class CPlayer {
    void Render(ID3D12GraphicsCommandList* pCommandList) {
        // PSO 설정
        pCommandList->SetPipelineState(m_pMeshPSO);
        pCommandList->SetGraphicsRootSignature(m_pRootSignature);
        
        // 루트 파라미터 설정
        pCommandList->SetGraphicsRootConstantBufferView(0, m_pConstantBuffer);
        
        // 그리기
        pCommandList->DrawIndexedInstanced(...);
    }
};

class CEnemy {
    void Render(ID3D12GraphicsCommandList* pCommandList) {
        // 여기서도 PSO 설정
        pCommandList->SetPipelineState(m_pMeshPSO);  // 플레이어와 같은 PSO인데 또 설정!
        
        // 앗! 루트 시그니처 설정 깜빡함!
        // pCommandList->SetGraphicsRootSignature(...);  
        
        pCommandList->DrawIndexedInstanced(...);  // 💥 크래시 또는 이상한 렌더링!
    }
};

// 게임 루프
void GameLoop() {
    player->Render(pCommandList);   // PSO, RootSig 설정됨
    enemy->Render(pCommandList);    // RootSig 설정 안 함 → 문제 발생!
}
```

```cpp
// 해소
class CPlayer {
    // 렌더링 데이터만 제공
    XMMATRIX GetWorldMatrix() { return m_matWorld; }
    void* GetHandle() { return this; }
};

// 렌더링 로직은 한 곳에서만!
DWORD CRenderQueue::Process(ID3D12GraphicsCommandList* pCommandList) {
    // 모든 상태 설정을 한 곳에서 관리
    pCommandList->SetPipelineState(m_pMeshPSO);
    pCommandList->SetGraphicsRootSignature(m_pRootSignature);
    
    while (const RENDER_ITEM* pItem = Dispatch()) {
        if (pItem->Type == RENDER_ITEM_TYPE_MESH_OBJ) {
            // 일관된 방식으로 렌더링
            pCommandList->SetGraphicsRootConstantBufferView(0, ...);
            pCommandList->DrawIndexedInstanced(...);
        }
    }
}
```

```cpp
// 문제상황2
// D3D12의 CommandList는 thread-safe하지 않음!

std::thread thread1([&]() {
    player->Render(pCommandList);  // 💥 동시 접근!
});

std::thread thread2([&]() {
    enemy->Render(pCommandList);   // 💥 동시 접근!
});

// 결과: 
// - CommandList 내부 상태 꼬임
// - GPU 커맨드 손상
// - 크래시 또는 렌더링 깨짐
```

```cpp
// 문제상황3

class CUI {
    void Render(ID3D12GraphicsCommandList* pCommandList) {
        // UI는 항상 맨 마지막에 그려져야 하는데...
        RenderSprite(pCommandList);
    }
};

void GameLoop() {
    // 호출 순서가 코드 순서에 의존
    ui->Render(pCommandList);        // UI가 먼저?
    player->Render(pCommandList);    // 플레이어가 UI 위에 그려짐! 💥
    enemy->Render(pCommandList);
    
    // UI를 마지막으로 옮기려면 코드 순서를 바꿔야 함
    // 복잡한 씬에서는 관리가 악몽!
}
```

```cpp
// 문제상황4

// 특정 객체만 렌더링 끄기 어려움
void GameLoop() {
    player->Render(pCommandList);
    
    // 디버깅: Enemy 렌더링만 끄고 싶다면?
    if (!debugDisableEnemy) {  // 코드 곳곳에 if문 추가해야 함
        enemy->Render(pCommandList);
    }
    
    weapon->Render(pCommandList);
    
    // 렌더링 통계 수집?
    // → 각 Render() 함수마다 통계 코드 추가해야 함
}
```

---

## 실제구현

```cpp
BOOL CD3D12Renderer::Initialize(HWND hWnd, BOOL bEnableDebugLayer, BOOL bEnableGBV)
{
    // ...
	m_pRenderQueue = new CRenderQueue;
	m_pRenderQueue->Initialize(this, 8192);
```

```cpp
void CGameObject::Render()
{
	if (m_pMeshObj)
	{
		m_pRenderer->RenderMeshObject(m_pMeshObj, &m_matWorld);
	}
}
```

```cpp
void CD3D12Renderer::RenderMeshObject(void* pMeshObjHandle, const XMMATRIX* pMatWorld)
{
	RENDER_ITEM item;
	item.Type = RENDER_ITEM_TYPE_MESH_OBJ;
	item.pObjHandle = pMeshObjHandle;           // Mesh Obj의 핸들을 담는다다
	item.MeshObjParam.matWorld = *pMatWorld;
	
	if (!m_pRenderQueue->Add(&item))
		__debugbreak();

    // ...
```

```cpp
void CD3D12Renderer::EndRender()
{
	ID3D12GraphicsCommandList* pCommandList = m_ppCommandList[m_dwCurContextIndex];

	// 렌더링큐에 쌓여있는 렌더링 요청을 한번에 처리
	m_pRenderQueue->Process(pCommandList);

	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargets[m_uiRenderTargetIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	pCommandList->Close();

	ID3D12CommandList* ppCommandLists[] = { pCommandList };
	m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	m_pRenderQueue->Reset();
}
```

```cpp
DWORD CRenderQueue::Process(ID3D12GraphicsCommandList* pCommandList)
{
	ID3D12Device5* pD3DDevice = m_pRenderer->INL_GetD3DDevice();

	DWORD dwItemCount = 0;
	const RENDER_ITEM* pItem = nullptr;
	while (pItem = Dispatch())
	{
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
		dwItemCount++;
	}
	return dwItemCount;
}
```