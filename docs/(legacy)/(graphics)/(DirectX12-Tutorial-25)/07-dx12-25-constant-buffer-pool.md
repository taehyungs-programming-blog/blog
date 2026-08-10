---
layout: default
title: "07. Constant buffer pool"
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

* 현재 코드에서 아래와 같이 동작하면 어떻게 될까?

```cpp
void RunGame()
{
	// ...

	// rendering objects
        // object를 두 개 그려보자
	g_pRenderer->RenderMeshObject(g_pMeshObj, g_fOffsetX, g_fOffsetY);

    g_pRenderer->RenderMeshObject(g_pMeshObj, g_fOffsetX, g_fOffsetY + 0.01);
```

```cpp
void CD3D12Renderer::RenderMeshObject(void* pMeshObjHandle, float x_offset, float y_offset)
{
	CBasicMeshObject* pMeshObj = (CBasicMeshObject*)pMeshObjHandle;
	XMFLOAT2	Pos = { x_offset, y_offset };
	pMeshObj->Draw(m_pCommandList, &Pos);
}
```

```cpp
void CBasicMeshObject::Draw(ID3D12GraphicsCommandList* pCommandList, const XMFLOAT2* pPos)
{
	m_pSysConstBufferDefault->offset.x = pPos->x;
	m_pSysConstBufferDefault->offset.y = pPos->y;

	// set RootSignature
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	pCommandList->SetDescriptorHeaps(1, &m_pDescritorHeap);

	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptorTable(m_pDescritorHeap->GetGPUDescriptorHandleForHeapStart());
	pCommandList->SetGraphicsRootDescriptorTable(0, gpuDescriptorTable);

	pCommandList->SetPipelineState(m_pPipelineState);
	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);

    // 여기가 두 번 호출되니 2개가 그려지려나?
	pCommandList->DrawInstanced(3, 1, 0, 0);
}
```

* 결론은 하나만 그려진다.
* 전달된 descriptor를 덮어쓴다는 말이겠지?
    * Dx11에서는 이렇게 그리면 덮어쓰는대신 새로운 리소스를 할당해서 두 번 그려준다

---

## 그래서 준비했다 Constant Buffer Pool

### 핵심 아이디어

* Constant Buffer를 매번 생성/삭제하지 않고 재활용하고 싶은데, D3D12의 제약이 있다

* **Shader Visible Descriptor Heap**은 크기 제한이 있고 생성 비용이 비싸다
* 모든 Constant Buffer를 Shader Visible Heap에 올리면 비효율적입니다

**해결책**: 2단계 구조로 분리
* **준비 공간 (CSimpleConstantBufferPool)**: 실제 데이터를 저장하는 큰 창고
* **사용 공간 (CDescriptorPool)**: Shader가 접근할 수 있는 작은 진열대

### 역할 구분

| 항목 | CSimpleConstantBufferPool | CDescriptorPool |
|------|---------------------------|-----------------|
| **실제 데이터 (Upload Heap)** | ✅ 생성 | ❌ 없음 |
| **Descriptor Heap** | Non-Visible (Shader 접근 불가) | Shader Visible (Shader 접근 가능) |
| **역할** | 모든 CBV를 미리 생성해두는 창고 | 필요한 것만 복사해서 쓰는 진열대 |

### CDescriptorPool 구현

```cpp
BOOL CDescriptorPool::Initialize(ID3D12Device5* pD3DDevice, UINT MaxDescriptorCount)
{
	BOOL bResult = FALSE;
	m_pD3DDevice = pD3DDevice;
	
	m_MaxDescriptorCount = MaxDescriptorCount;
	m_srvDescriptorSize = m_pD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	// create descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC commonHeapDesc = {};
	commonHeapDesc.NumDescriptors = m_MaxDescriptorCount;
	commonHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	commonHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  // ⭐ Shader가 접근 가능!
	if (FAILED(m_pD3DDevice->CreateDescriptorHeap(&commonHeapDesc, IID_PPV_ARGS(&m_pDescritorHeap))))
	{
		__debugbreak();
		goto lb_return;
	}
	
	// 선두 번지를 기록해 둔다 (실제 데이터 없이 빈 공간만 생성)
	m_cpuDescriptorHandle = m_pDescritorHeap->GetCPUDescriptorHandleForHeapStart();
	m_gpuDescriptorHandle = m_pDescritorHeap->GetGPUDescriptorHandleForHeapStart();
	bResult = TRUE;
	
lb_return:
	return bResult;
}
```

**포인트**: 
* 실제 GPU 메모리(Resource)는 생성하지 않음
* Descriptor Heap만 생성 (빈 슬롯만 확보)
* Shader Visible 플래그로 GPU가 접근 가능하게 설정

### CSimpleConstantBufferPool 구현

```cpp
BOOL CSimpleConstantBufferPool::Initialize(ID3D12Device* pD3DDevice, UINT SizePerCBV, UINT MaxCBVNum)
{
	m_MaxCBVNum = MaxCBVNum;
	m_SizePerCBV = SizePerCBV;
	UINT ByteWidth = SizePerCBV * m_MaxCBVNum;
	
	// ⭐ 실제 GPU 메모리 생성 (Upload Heap)
	if (FAILED(pD3DDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(ByteWidth),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pResource))))
	{
		__debugbreak();
	}
	
	// create descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = m_MaxCBVNum;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // ⭐ Non-Visible (Shader 접근 불가)
	if (FAILED(pD3DDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pCBVHeap))))
	{
		__debugbreak();
	}
	
	// CPU에서 GPU 메모리에 직접 쓸 수 있도록 매핑
	CD3DX12_RANGE readRange(0, 0);
	m_pResource->Map(0, &readRange, reinterpret_cast<void**>(&m_pSystemMemAddr));
	
	m_pCBContainerList = new CB_CONTAINER[m_MaxCBVNum];
	
	// 모든 CBV를 미리 생성
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_pResource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = m_SizePerCBV;
	
	UINT8* pSystemMemPtr = m_pSystemMemAddr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE heapHandle(m_pCBVHeap->GetCPUDescriptorHandleForHeapStart());
	UINT DescriptorSize = pD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	for (DWORD i = 0; i < m_MaxCBVNum; i++)
	{
		// Descriptor 생성 (GPU 주소 + 크기 정보)
		pD3DDevice->CreateConstantBufferView(&cbvDesc, heapHandle);
		
		m_pCBContainerList[i].CBVHandle = heapHandle;
		m_pCBContainerList[i].pGPUMemAddr = cbvDesc.BufferLocation;
		m_pCBContainerList[i].pSystemMemAddr = pSystemMemPtr;
		
		heapHandle.Offset(1, DescriptorSize);
		cbvDesc.BufferLocation += m_SizePerCBV;
		pSystemMemPtr += m_SizePerCBV;
	}
	return TRUE;
}
```

**포인트**:
* 실제 GPU Upload Heap 메모리를 생성
* CPU에서 직접 쓸 수 있도록 Map
* 모든 CBV Descriptor를 미리 생성 (Non-Visible Heap에)
* 초기화 시 한 번만 생성하고 계속 재사용

### Draw 시 동작 과정

```cpp
void CBasicMeshObject::Draw(ID3D12GraphicsCommandList* pCommandList, const XMFLOAT2* pPos)
{
	// 각각의 draw() 작업의 무결성을 보장하려면 매번 다른 영역을 사용해야 함
	ID3D12Device5* pD3DDeivce = m_pRenderer->INL_GetD3DDevice();
	UINT srvDescriptorSize = m_pRenderer->INL_GetSrvDescriptorSize();
	CDescriptorPool* pDescriptorPool = m_pRenderer->INL_GetDescriptorPool();
	ID3D12DescriptorHeap* pDescriptorHeap = pDescriptorPool->INL_GetDescriptorHeap();
	CSimpleConstantBufferPool* pConstantBufferPool = m_pRenderer->INL_GetConstantBufferPool();
	
	// 1️⃣ Shader Visible Heap에서 빈 슬롯 할당
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptorTable = {};
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptorTable = {};
	if (!pDescriptorPool->AllocDescriptorTable(&cpuDescriptorTable, &gpuDescriptorTable, DESCRIPTOR_COUNT_FOR_DRAW))
	{
		__debugbreak();
	}
	
	// 2️⃣ Constant Buffer Pool에서 CBV 할당 (이미 생성된 것 중 하나 가져옴)
	CB_CONTAINER* pCB = pConstantBufferPool->Alloc();
	if (!pCB)
	{
		__debugbreak();
	}
	CONSTANT_BUFFER_DEFAULT* pConstantBufferDefault = (CONSTANT_BUFFER_DEFAULT*)pCB->pSystemMemAddr;
	
	// 3️⃣ Upload Heap에 실제 데이터 쓰기
	pConstantBufferDefault->offset.x = pPos->x;
	pConstantBufferDefault->offset.y = pPos->y;
	
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);
	pCommandList->SetDescriptorHeaps(1, &pDescriptorHeap);
	
	// 4️⃣ ⭐ 중요! Descriptor만 복사 (실제 데이터 복사 아님!)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(cpuDescriptorTable, BASIC_MESH_DESCRIPTOR_INDEX_CBV, srvDescriptorSize);
	pD3DDeivce->CopyDescriptorsSimple(1, cbvDest, pCB->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	// 5️⃣ GPU에게 Shader Visible Descriptor Table 위치 알려주기
	pCommandList->SetGraphicsRootDescriptorTable(0, gpuDescriptorTable);
	
	pCommandList->SetPipelineState(m_pPipelineState);
	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	pCommandList->DrawInstanced(3, 1, 0, 0);
}
```

### 🔍 CopyDescriptorsSimple의 진실

**중요**: `CopyDescriptorsSimple`은 실제 Constant Buffer 데이터를 복사하는 게 아닙니다!

**복사되는 것** (약 32바이트):
* GPU Virtual Address (메모리 주소)
* Buffer Size (버퍼 크기)
* 기타 메타데이터

**복사되지 않는 것**:
* ❌ Constant Buffer의 실제 데이터 (`offset.x`, `offset.y` 등)
* ❌ Upload Heap 메모리 내용

```
[메모리 구조]

Upload Heap (실제 데이터)
┌────────────────────────┐
│ CBV #0: {x:0.5, y:0.0} │ ← 여기 그대로 있음!
└────────────────────────┘
         ↑
         │ Descriptor가 이 주소를 가리킴
         │
    ┌────┴────┐
    │         │
    ↓         ↓
Non-Visible   Shader Visible
Descriptor    Descriptor
┌─────────┐   ┌─────────┐
│주소:0x12│──>│주소:0x12│ ← 주소 정보만 복사
│크기:256 │   │크기:256 │
└─────────┘   └─────────┘
```
