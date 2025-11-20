---
layout: default
title: "08. Draw시 Texture 교체하는 효율적 방법"
parent: "(DirectX12 25년 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---


## 1. CIndexCreator 사용 방법

### 1.1 개념
`CIndexCreator`는 **인덱스 풀(Index Pool)** 관리자입니다. 사용 가능한 인덱스를 효율적으로 할당하고 반환하는 역할을 합니다.

### 1.2 내부 구조
```cpp
class CIndexCreator
{
    DWORD*  m_pdwIndexTable;      // 인덱스 배열
    DWORD   m_dwMaxNum;            // 최대 개수
    DWORD   m_dwAllocatedCount;    // 현재 할당된 개수
};
```

### 1.3 동작 원리

#### 초기화 (Initialize)
```cpp
BOOL CIndexCreator::Initialize(DWORD dwNum)
{
    m_pdwIndexTable = new DWORD[dwNum];
    m_dwMaxNum = dwNum;
    
    // 배열에 0, 1, 2, 3, ... dwNum-1 순서로 저장
    for (DWORD i = 0; i < m_dwMaxNum; i++)
    {
        m_pdwIndexTable[i] = i;
    }
    return TRUE;
}
```

**초기 상태 예시 (dwNum = 5)**
```
m_pdwIndexTable: [0, 1, 2, 3, 4]
m_dwAllocatedCount: 0
```

#### 할당 (Alloc)
```cpp
DWORD CIndexCreator::Alloc()
{
    if (m_dwAllocatedCount >= m_dwMaxNum)
        return -1;  // 실패
    
    DWORD dwResult = m_pdwIndexTable[m_dwAllocatedCount];
    m_dwAllocatedCount++;
    return dwResult;
}
```

**할당 과정 시뮬레이션**
```
[초기]
배열: [0, 1, 2, 3, 4]
할당된 개수: 0

[Alloc() 호출 #1] → 반환값: 0
배열: [0, 1, 2, 3, 4]
할당된 개수: 1

[Alloc() 호출 #2] → 반환값: 1
배열: [0, 1, 2, 3, 4]
할당된 개수: 2

[Alloc() 호출 #3] → 반환값: 2
배열: [0, 1, 2, 3, 4]
할당된 개수: 3
```

#### 해제 (Free)
```cpp
void CIndexCreator::Free(DWORD dwIndex)
{
    m_dwAllocatedCount--;
    m_pdwIndexTable[m_dwAllocatedCount] = dwIndex;
}
```

**해제 과정 시뮬레이션**
```
[현재 상태]
배열: [0, 1, 2, 3, 4]
할당된 개수: 3

[Free(1) 호출]
배열: [0, 1, 1, 3, 4]  ← 인덱스 2 위치에 1을 저장
할당된 개수: 2

[다음 Alloc() 호출] → 반환값: 1 (재사용!)
배열: [0, 1, 1, 3, 4]
할당된 개수: 3
```

### 1.4 핵심 특징
- **O(1) 시간 복잡도**: 할당과 해제 모두 상수 시간
- **스택 방식**: LIFO (Last In First Out) - 가장 최근에 해제된 인덱스를 먼저 재사용
- **메모리 효율적**: 별도의 연결 리스트나 복잡한 자료구조 불필요

---

## 2. Draw Call에서 텍스처 교환 방식

### 2.1 Draw 호출 흐름

```cpp
void CD3D12Renderer::RenderMeshObject(void* pMeshObjHandle, 
                                       float x_offset, 
                                       float y_offset, 
                                       void* pTexHandle)
{
    D3D12_CPU_DESCRIPTOR_HANDLE srv = {};
    if (pTexHandle)
    {
        // TEXTURE_HANDLE에서 srv(Shader Resource View) 추출
        srv = ((TEXTURE_HANDLE*)pTexHandle)->srv;
    }
    
    XMFLOAT2 Pos = { x_offset, y_offset };
    pMeshObj->Draw(m_pCommandList, &Pos, srv);  // ← Draw 호출
}
```

### 2.2 Draw 함수 내부 - 텍스처 교환 메커니즘

```cpp
void CBasicMeshObject::Draw(ID3D12GraphicsCommandList* pCommandList, 
                             const XMFLOAT2* pPos, 
                             D3D12_CPU_DESCRIPTOR_HANDLE srv)
{
    // 1. 매 Draw마다 새로운 Descriptor Table 할당
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescriptorTable = {};
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescriptorTable = {};
    
    pDescriptorPool->AllocDescriptorTable(&cpuDescriptorTable, 
                                           &gpuDescriptorTable, 
                                           DESCRIPTOR_COUNT_FOR_DRAW);  // 2개 (CBV + SRV)
    
    // 2. Constant Buffer 할당 및 데이터 설정
    CB_CONTAINER* pCB = pConstantBufferPool->Alloc();
    CONSTANT_BUFFER_DEFAULT* pConstantBufferDefault = 
        (CONSTANT_BUFFER_DEFAULT*)pCB->pSystemMemAddr;
    
    pConstantBufferDefault->offset.x = pPos->x;
    pConstantBufferDefault->offset.y = pPos->y;
    
    // 3. Root Signature 및 Descriptor Heap 설정
    pCommandList->SetGraphicsRootSignature(m_pRootSignature);
    pCommandList->SetDescriptorHeaps(1, &pDescriptorHeap);
    
    // 4. CBV를 Descriptor Table에 복사
    CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(cpuDescriptorTable, 
                                           BASIC_MESH_DESCRIPTOR_INDEX_CBV,  // 0
                                           srvDescriptorSize);
    pD3DDeivce->CopyDescriptorsSimple(1, cbvDest, pCB->CBVHandle, 
                                       D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    
    // 5. ★ 텍스처(SRV)를 Descriptor Table에 복사 ★
    if (srv.ptr)
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE srvDest(cpuDescriptorTable, 
                                               BASIC_MESH_DESCRIPTOR_INDEX_TEX,  // 1
                                               srvDescriptorSize);
        pD3DDeivce->CopyDescriptorsSimple(1, srvDest, srv, 
                                           D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
    
    // 6. GPU에 Descriptor Table 설정
    pCommandList->SetGraphicsRootDescriptorTable(0, gpuDescriptorTable);
    
    // 7. 실제 드로우
    pCommandList->SetPipelineState(m_pPipelineState);
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
    pCommandList->DrawInstanced(3, 1, 0, 0);
}
```

### 2.3 텍스처 교환 핵심 포인트

#### Descriptor Table 구조
```
Root Parameter[0] = Descriptor Table
├─ [0] CBV (Constant Buffer)  ← 매 Draw마다 다른 위치 정보
└─ [1] SRV (Texture)           ← 매 Draw마다 다른 텍스처
```

#### 교환 방식의 핵심
1. **매 Draw마다 새로운 Descriptor Table 할당**
   - `CDescriptorPool::AllocDescriptorTable()`을 통해 임시 Descriptor Table 획득
   
2. **원본 Descriptor 복사**
   - `CopyDescriptorsSimple()` 사용
   - **CSingleDescriptorAllocator에서 할당받은 srv (원본)** → **임시 Descriptor Table (사본)**
   
3. **동적 바인딩**
   - `SetGraphicsRootDescriptorTable()` 호출 시점에 GPU에 전달
   - 각 Draw Call마다 다른 텍스처를 참조 가능

### 2.4 텍스처 공유의 의미

```
[텍스처 리소스]
┌─────────────────────────────┐
│ Texture Resource (GPU 메모리)│  ← 한 번만 생성
│  - ID3D12Resource            │
└─────────────────────────────┘
              ↓
[CSingleDescriptorAllocator]
┌─────────────────────────────┐
│ Non-Shader-Visible Heap      │  ← 원본 Descriptor 저장
│  - srv (CPU Handle)          │     (프레임 간 유지)
└─────────────────────────────┘
              ↓ CopyDescriptorsSimple (매 Draw마다)
[CDescriptorPool]
┌─────────────────────────────┐
│ Shader-Visible Heap          │  ← 임시 Descriptor Table
│  - 매 프레임 Reset           │     (GPU가 실제로 읽음)
└─────────────────────────────┘
```

**공유 방식**
- **텍스처 리소스**: 한 번만 생성, 여러 오브젝트가 공유
- **원본 Descriptor (srv)**: CSingleDescriptorAllocator에 영구 저장
- **Draw 시**: 원본 Descriptor를 임시 테이블에 복사하여 사용

---

## 3. CSingleDescriptorAllocator 파이프라인

### 3.1 전체 아키텍처

```
┌─────────────────────────────────────────────────────────────┐
│                   D3D12 렌더링 시스템                         │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
┌───────▼────────┐  ┌─────────▼────────┐  ┌────────▼────────┐
│ CSingleDesc-   │  │ CDescriptorPool  │  │ CConstantBuffer │
│ riptorAllocator│  │                  │  │ Pool            │
│                │  │                  │  │                 │
│ (Non-Shader-   │  │ (Shader-Visible) │  │ (Constant Data) │
│  Visible)      │  │                  │  │                 │
└────────────────┘  └──────────────────┘  └─────────────────┘
```

### 3.2 초기화 과정

#### CD3D12Renderer::Initialize()
```cpp
// 1. SingleDescriptorAllocator 생성 및 초기화
m_pSingleDescriptorAllocator = new CSingleDescriptorAllocator;
m_pSingleDescriptorAllocator->Initialize(m_pD3DDevice, 
                                          MAX_DESCRIPTOR_COUNT,  // 4096
                                          D3D12_DESCRIPTOR_HEAP_FLAG_NONE);  // ← Non-Shader-Visible
```

#### CSingleDescriptorAllocator::Initialize()
```cpp
BOOL CSingleDescriptorAllocator::Initialize(ID3D12Device* pDevice, 
                                             DWORD dwMaxCount, 
                                             D3D12_DESCRIPTOR_HEAP_FLAGS Flags)
{
    m_pD3DDevice = pDevice;
    
    // 1. Descriptor Heap 생성
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = dwMaxCount;  // 4096
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = Flags;  // D3D12_DESCRIPTOR_HEAP_FLAG_NONE
    
    m_pD3DDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pHeap));
    
    // 2. IndexCreator 초기화
    m_IndexCreator.Initialize(dwMaxCount);  // 0~4095 인덱스 준비
    
    // 3. Descriptor 크기 저장
    m_DescriptorSize = m_pD3DDevice->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    
    return TRUE;
}
```

### 3.3 텍스처 생성 파이프라인

```
[CreateTiledTexture]
       ↓
1. 텍스처 이미지 데이터 생성 (CPU)
       ↓
2. GPU 텍스처 리소스 생성 (ID3D12Resource)
       ↓
3. ★ CSingleDescriptorAllocator::AllocDescriptorHandle() ★
       ↓
4. CreateShaderResourceView (SRV 생성)
       ↓
5. TEXTURE_HANDLE 반환 (리소스 + SRV)
```

#### 상세 코드
```cpp
void* CD3D12Renderer::CreateTiledTexture(UINT TexWidth, UINT TexHeight, 
                                          DWORD r, DWORD g, DWORD b)
{
    ID3D12Resource* pTexResource = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE srv = {};
    
    // 1. 텍스처 데이터 생성 (체크보드 패턴)
    BYTE* pImage = (BYTE*)malloc(TexWidth * TexHeight * 4);
    // ... 이미지 데이터 채우기 ...
    
    // 2. GPU 텍스처 리소스 생성
    m_pResourceManager->CreateTexture(&pTexResource, TexWidth, TexHeight, 
                                       TexFormat, pImage);
    
    // 3. ★ Descriptor Handle 할당 ★
    if (m_pSingleDescriptorAllocator->AllocDescriptorHandle(&srv))
    {
        // 4. SRV 생성
        D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
        SRVDesc.Format = TexFormat;
        SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        SRVDesc.Texture2D.MipLevels = 1;
        
        m_pD3DDevice->CreateShaderResourceView(pTexResource, &SRVDesc, srv);
        
        // 5. 핸들 반환
        TEXTURE_HANDLE* pTexHandle = new TEXTURE_HANDLE;
        pTexHandle->pTexResource = pTexResource;
        pTexHandle->srv = srv;  // ← 이 srv를 Draw에서 사용!
        return pTexHandle;
    }
}
```

### 3.4 AllocDescriptorHandle 내부 동작

```cpp
BOOL CSingleDescriptorAllocator::AllocDescriptorHandle(
    D3D12_CPU_DESCRIPTOR_HANDLE* pOutCPUHandle)
{
    // 1. IndexCreator로부터 인덱스 할당
    DWORD dwIndex = m_IndexCreator.Alloc();
    if (-1 == dwIndex)
        return FALSE;  // 실패
    
    // 2. 인덱스를 사용해 Descriptor Handle 계산
    CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(
        m_pHeap->GetCPUDescriptorHandleForHeapStart(),  // 시작 주소
        dwIndex,                                         // 오프셋 인덱스
        m_DescriptorSize);                              // 한 개의 크기
    
    // 3. 반환
    *pOutCPUHandle = DescriptorHandle;
    return TRUE;
}
```

**메모리 레이아웃**
```
[Descriptor Heap - 4096 slots]
┌────┬────┬────┬────┬─────────┬────┐
│ 0  │ 1  │ 2  │ 3  │   ...   │4095│
└────┴────┴────┴────┴─────────┴────┘
  ↑    ↑    ↑
  │    │    └─ Texture3
  │    └────── Texture2
  └─────────── Texture1

각 슬롯 크기: m_DescriptorSize (보통 32바이트)
```

### 3.5 텍스처 삭제 파이프라인

```cpp
void CD3D12Renderer::DeleteTexture(void* pHandle)
{
    TEXTURE_HANDLE* pTexHandle = (TEXTURE_HANDLE*)pHandle;
    
    // 1. GPU 리소스 해제
    pTexHandle->pTexResource->Release();
    
    // 2. ★ Descriptor Handle 반환 ★
    m_pSingleDescriptorAllocator->FreeDescriptorHandle(pTexHandle->srv);
    
    // 3. 핸들 구조체 삭제
    delete pTexHandle;
}
```

#### FreeDescriptorHandle 내부
```cpp
void CSingleDescriptorAllocator::FreeDescriptorHandle(
    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle)
{
    // 1. CPU Handle → 인덱스 역계산
    D3D12_CPU_DESCRIPTOR_HANDLE base = m_pHeap->GetCPUDescriptorHandleForHeapStart();
    DWORD dwIndex = (DWORD)(DescriptorHandle.ptr - base.ptr) / m_DescriptorSize;
    
    // 2. IndexCreator에 인덱스 반환
    m_IndexCreator.Free(dwIndex);
}
```

### 3.6 전체 데이터 흐름

```
┌─────────────────────────────────────────────────────────────────┐
│                        프레임 시작                                │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ BeginRender()                                                     │
│  - Command List Reset                                            │
│  - Render Target 설정                                            │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ RenderMeshObject() [반복]                                        │
│   ↓                                                              │
│   CBasicMeshObject::Draw()                                       │
│   ├─ CDescriptorPool::AllocDescriptorTable() (임시)             │
│   ├─ CConstantBufferPool::Alloc() (임시)                        │
│   ├─ CopyDescriptorsSimple() ← ★ CSingleDescriptorAllocator의  │
│   │                               srv를 임시 테이블에 복사 ★      │
│   └─ DrawInstanced()                                             │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ EndRender()                                                       │
│  - Command List Close                                            │
│  - ExecuteCommandLists                                           │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ Present()                                                         │
│  - Swap Chain Present                                            │
│  - Fence & Wait                                                  │
│  - ★ CDescriptorPool::Reset() ★ (임시 테이블 초기화)            │
│  - ★ CConstantBufferPool::Reset() ★                             │
│  - ※ CSingleDescriptorAllocator는 Reset 안함! (영구 저장) ※    │
└─────────────────────────────────────────────────────────────────┘
```

### 3.7 핵심 설계 원칙

#### 두 종류의 Descriptor Heap 분리

| 구분 | CSingleDescriptorAllocator | CDescriptorPool |
|------|---------------------------|-----------------|
| **용도** | 텍스처 원본 Descriptor 저장 | 매 Draw용 임시 테이블 |
| **Heap Type** | Non-Shader-Visible | Shader-Visible |
| **생명주기** | 프레임 간 유지 | 매 프레임 Reset |
| **할당 방식** | CIndexCreator (인덱스 기반) | Linear Allocator |
| **GPU 접근** | 불가 (CPU 전용) | 가능 (GPU가 읽음) |

#### 왜 이렇게 분리하는가?

1. **성능 최적화**
   - Shader-Visible Heap은 GPU가 읽을 수 있어야 하므로 상대적으로 느림
   - 원본 Descriptor는 Non-Shader-Visible에 두고, 필요할 때만 복사

2. **메모리 효율**
   - 텍스처는 한 번만 저장하고, 여러 번 사용
   - 임시 Descriptor Table은 매 프레임 재활용

3. **유연성**
   - 같은 텍스처를 여러 오브젝트가 공유 가능
   - Draw마다 다른 텍스처 조합 가능

### 3.8 사용 예시 시나리오

```cpp
// 초기화 단계
TEXTURE_HANDLE* pTex1 = CreateTiledTexture(256, 256, 255, 0, 0);    // 빨강
TEXTURE_HANDLE* pTex2 = CreateTiledTexture(256, 256, 0, 255, 0);    // 초록
CBasicMeshObject* pMesh1 = CreateBasicMeshObject();
CBasicMeshObject* pMesh2 = CreateBasicMeshObject();

// 렌더링 루프
BeginRender();
    RenderMeshObject(pMesh1, -0.5f, 0.0f, pTex1);  // 왼쪽에 빨간 텍스처
    RenderMeshObject(pMesh2, +0.5f, 0.0f, pTex2);  // 오른쪽에 초록 텍스처
    RenderMeshObject(pMesh1, 0.0f, +0.5f, pTex2);  // 위쪽에 초록 텍스처 (재사용!)
EndRender();
Present();

// 정리 단계
DeleteTexture(pTex1);
DeleteTexture(pTex2);
DeleteBasicMeshObject(pMesh1);
DeleteBasicMeshObject(pMesh2);
```

**메모리 상태**
```
[CSingleDescriptorAllocator Heap]
Index 0: pTex1->srv (빨강 텍스처 Descriptor)
Index 1: pTex2->srv (초록 텍스처 Descriptor)
         ... 유지됨 (프레임 간) ...

[CDescriptorPool Heap - 매 Draw마다]
Draw #1: Table[0]=CBV(mesh1,pos1), Table[1]=SRV(pTex1)  ← pTex1->srv 복사
Draw #2: Table[0]=CBV(mesh2,pos2), Table[1]=SRV(pTex2)  ← pTex2->srv 복사
Draw #3: Table[0]=CBV(mesh1,pos3), Table[1]=SRV(pTex2)  ← pTex2->srv 재사용!
         ... Present 후 Reset ...
```

