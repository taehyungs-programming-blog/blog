---
layout: default
title: "16. (복습) Descriptor Heap 전체 사용 절차 + Command Queue와 SwapChain"
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

> 사실은 복습에 가까움

---

## (복습1) Descriptor Heap 전체 사용 절차

### 1단계: Descriptor Heap 생성

#### CPU-only Heap (저장 용도)

* 메모리 위치: 시스템 메모리 (RAM)
* 접근: CPU만 가능

```cpp
// TextureManager나 리소스 관리자에서 생성
D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
heapDesc.NumDescriptors = 1000;  // descriptor 개수
heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // CPU만 접근

ID3D12DescriptorHeap* pCPUHeap = nullptr;
pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&pCPUHeap));

// Heap의 시작 주소 얻기
D3D12_CPU_DESCRIPTOR_HANDLE cpuStart = pCPUHeap->GetCPUDescriptorHandleForHeapStart();
```

#### GPU-visible Heap (렌더링 시 사용)

* 메모리 위치: GPU 메모리 (VRAM) 또는 GPU-accessible 메모리
* 접근: CPU/GPU 모두 가능

```cpp
// Renderer에서 생성
D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
heapDesc.NumDescriptors = 100;  // 제한적 (보통 CBV_SRV_UAV는 1M개까지)
heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  // GPU가 읽을 수 있음!

ID3D12DescriptorHeap* pGPUHeap = nullptr;
pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&pGPUHeap));

// CPU/GPU 주소 모두 얻기
D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = pGPUHeap->GetCPUDescriptorHandleForHeapStart();
D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = pGPUHeap->GetGPUDescriptorHandleForHeapStart();
```

### 2단계: 리소스 생성 및 View(Descriptor) 만들기

#### 텍스처 로드

* 메모리 위치: GPU 메모리 (VRAM)

```cpp
// TextureManager::CreateTexture()
ID3D12Resource* pTextureResource = nullptr;

// 2-1. GPU에 텍스처 리소스 생성
D3D12_RESOURCE_DESC texDesc = {};
texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
texDesc.Width = 1024;
texDesc.Height = 1024;
texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
// ...

pDevice->CreateCommittedResource(
    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),  // GPU 메모리
    D3D12_HEAP_FLAG_NONE,
    &texDesc,
    D3D12_RESOURCE_STATE_COPY_DEST,
    nullptr,
    IID_PPV_ARGS(&pTextureResource)
);
```

#### SRV (Shader Resource View) 생성

```cpp
// 2-2. CPU-only heap에 SRV descriptor 생성
D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
srvDesc.Texture2D.MipLevels = 1;

// CPU-only heap의 특정 슬롯에 SRV 생성
CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(cpuHeapStart, index, descriptorSize);

pDevice->CreateShaderResourceView(
    pTextureResource,  // 실제 텍스처 리소스
    &srvDesc,
    srvHandle          // 이 위치에 descriptor 작성
);

// TEXTURE_HANDLE 구조체에 저장
pTexHandle->srv = srvHandle;  // CPU descriptor handle 저장
pTexHandle->pTexture = pTextureResource;
```

### 3단계: 렌더링 전 - Descriptor Table 구성

#### GPU-visible heap으로 복사

```cpp
// BasicMeshObject::UpdateDescriptorTable() 같은 함수에서

// GPU-visible heap의 시작 위치 계산
CD3DX12_CPU_DESCRIPTOR_HANDLE destCPU(
    pGPUHeap->GetCPUDescriptorHandleForHeapStart(),
    objectIndex * DESCRIPTORS_PER_OBJECT,  // 이 오브젝트용 공간
    descriptorSize
);

// 3-1. CBV 복사 (CPU-only -> GPU-visible)
pDevice->CopyDescriptorsSimple(
    1,
    destCPU,           // [목적지] GPU-visible heap
    pCB->CBVHandle,    // [원본] CPU-only heap
    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
);
destCPU.Offset(1, descriptorSize);

// 3-2. 각 tri-group의 텍스처 SRV 복사
for (DWORD i = 0; i < m_dwTriGroupCount; i++)
{
    TEXTURE_HANDLE* pTexHandle = m_pTriGroupList[i].pTexHandle;
    
    pDevice->CopyDescriptorsSimple(
        1,
        destCPU,              // [목적지] GPU-visible heap (계속 증가)
        pTexHandle->srv,      // [원본] CPU-only heap
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );
    destCPU.Offset(1, descriptorSize);
}
```

### 4단계: 커맨드 리스트에 기록

```cpp
// Render() 함수에서
ID3D12GraphicsCommandList* pCommandList;

// 4-1. GPU-visible Descriptor Heap 설정
ID3D12DescriptorHeap* ppHeaps[] = { pGPUHeap };
pCommandList->SetDescriptorHeaps(1, ppHeaps);

// 4-2. Root Signature 설정
pCommandList->SetGraphicsRootSignature(pRootSignature);

// 4-3. Descriptor Table 바인딩
CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(
    pGPUHeap->GetGPUDescriptorHandleForHeapStart(),
    objectIndex * DESCRIPTORS_PER_OBJECT,
    descriptorSize
);

pCommandList->SetGraphicsRootDescriptorTable(
    0,          // Root parameter index (root signature에서 정의)
    gpuHandle   // GPU descriptor handle
);

// 4-4. 드로우 콜
pCommandList->DrawIndexedInstanced(...);
```

### 5단계: GPU 실행

```cpp
pCommandList->Close();
ID3D12CommandList* ppCommandLists[] = { pCommandList };
pCommandQueue->ExecuteCommandLists(1, ppCommandLists);
```

```cpp
// Pixel Shader에서
Texture2D g_texture0 : register(t0);  // Descriptor table의 slot 1
Texture2D g_texture1 : register(t1);  // Descriptor table의 slot 2
SamplerState g_sampler : register(s0);

cbuffer CB : register(b0)  // Descriptor table의 slot 0
{
    float4x4 worldMatrix;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    // GPU가 descriptor를 통해 텍스처에 접근
    float4 color = g_texture0.Sample(g_sampler, input.uv);
    return color;
}
```

---

## (복습2) Command Queue와 SwapChain

* 필요성

* Multi-Queue 환경이라 가정

```cpp
// ═══ 초기화 ═══
ID3D12CommandQueue* pGraphicsQueue;  // Direct Queue
ID3D12CommandQueue* pComputeQueue;   // Compute Queue
ID3D12CommandQueue* pCopyQueue;      // Copy Queue

// SwapChain은 Graphics Queue와 연결
pFactory->CreateSwapChainForHwnd(pGraphicsQueue, ...);

// ═══ 프레임 렌더링 ═══

// 1. Copy Queue: 비동기 텍스처 로딩 (백그라운드)
pCopyQueue->ExecuteCommandLists(1, ppCopyLists);

// 2. Compute Queue: Physics 계산 (백그라운드)
pComputeQueue->ExecuteCommandLists(1, ppComputeLists);

// 3. Graphics Queue: 실제 렌더링 (메인)
pGraphicsQueue->ExecuteCommandLists(1, ppGraphicsLists);

// 4. Present: Graphics Queue만 기다림!
pSwapChain->Present(1, 0);
// ↑ pGraphicsQueue의 작업만 완료되면 Present
// Copy, Compute Queue는 계속 실행 가능!
```

```
Time →
Copy Queue:    [텍스처 로딩.........................]
Compute Queue: [Physics 계산........]
Graphics Queue:[렌더링....]
                         ↑ Present는 여기만 기다림
                         
SwapChain: "Graphics Queue만 완료되면 OK!"
```

1. SwapChain: 연결된 Command Queue 확인
   → pGraphicsQueue
2. Queue의 마지막 작업 완료 여부 확인
   → 내부적으로 Fence 사용
3. 아직 실행 중이면:
   - V-Sync 타이밍까지 대기
   - GPU 작업 완료 대기
4. 완료되면:
   - Back Buffer ↔ Front Buffer 교환 (Flip)
   - 화면에 표시
5. GetCurrentBackBufferIndex() 업데이트
   → 다음 렌더링할 버퍼 인덱스 반환

```cpp
// ❌ 나쁜 설계 (가상)
pSwapChain->Present(1, 0);
// → 모든 Queue를 기다려야 한다면?
// → Copy/Compute가 느리면 Present도 느려짐!

// ✅ 좋은 설계 (실제)
pSwapChain->Present(1, 0);
// → Graphics Queue만 기다림
// → 다른 Queue는 계속 실행!
```

Command Queue의 역할:
- GPU 작업 제출
- GPU 실행 관리

SwapChain의 역할:
- 화면 출력 타이밍
- Buffer 교환
- V-Sync 관리

연결된 Command Queue를 통해:
- "화면 출력용 렌더링"이 어느 Queue인지 명확히 알 수 있음

```cpp
// GPU 0용 Queue
ID3D12CommandQueue* pQueue_GPU0;
pDevice_GPU0->CreateCommandQueue(&desc, IID_PPV_ARGS(&pQueue_GPU0));

// GPU 1용 Queue
ID3D12CommandQueue* pQueue_GPU1;
pDevice_GPU1->CreateCommandQueue(&desc, IID_PPV_ARGS(&pQueue_GPU1));

// SwapChain은 GPU 0의 Queue 사용
pFactory->CreateSwapChainForHwnd(pQueue_GPU0, ...);

// → SwapChain이 어느 GPU의 결과를 표시할지 명확!
```

### Tip) Present와 Fence

```cpp
// ═══ 방법 1: 명시적 Fence (권장) ═══
pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

// 명시적 Fence로 추적
fenceValue++;
pCommandQueue->Signal(pFence, fenceValue);

// Present (내부적으로도 동기화)
pSwapChain->Present(1, 0);

// 다음 프레임 전 대기
if (pFence->GetCompletedValue() < fenceValue)
{
    pFence->SetEventOnCompletion(fenceValue, fenceEvent);
    WaitForSingleObject(fenceEvent, INFINITE);
}

// ═══ 방법 2: Present만 의존 (비권장) ═══
pCommandQueue->ExecuteCommandLists(1, ppCommandLists);
pSwapChain->Present(1, 0);  // 내부적으로 대기

// 문제: 정확한 타이밍 제어 어려움
// Command Allocator를 언제 Reset해야 할지 불명확
```