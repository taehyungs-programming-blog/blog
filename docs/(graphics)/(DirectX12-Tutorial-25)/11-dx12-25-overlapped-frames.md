---
layout: default
title: "11. 중첩 렌더링"
parent: "(DirectX12 25년 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Overlapped/Multi-buffered Rendering

### 비동기식 중첩 렌더링 필요성

* 기존의 단순한 렌더링 루프는 GPU가 일하는 동안 CPU가 마냥 기다리는 비효율이 발생합니다.
* 기존 방식: `Rendering → Fence → Wait → Rendering` (직렬 처리)
* 문제점:
    * GPU가 명령을 수행하는 동안 CPU는 WaitForSingleObject 등에 묶여 아무것도 하지 못함 (마우스 이벤트 처리 불가 등).
    * CPU와 GPU의 병렬 처리 이점을 전혀 살리지 못함.
* 해결책:
    * GPU에 작업을 던져놓고(Submit), CPU는 즉시 다음 프레임이나 다다음 프레임의 준비를 시작한다.
    * 현재 프레임의 종료를 기다리는 것이 아니라, N 프레임 전의 작업이 끝났는지를 확인한다.
* 이를 위해 동시에 처리될 프레임 수만큼의 리소스(Constant Buffer, Command Allocator 등)를 미리 여러 개 준비해야 한다.

### 구현 핵심 개념

* 프레임 버퍼링 구조
    * `SWAP_CHAIN_FRAME_COUNT (3)`: 백버퍼의 개수 (Triple Buffering).
    * `MAX_PENDING_FRAME_COUNT (2)`: CPU가 GPU보다 앞서갈 수 있는 최대 프레임 수 `(SWAP_CHAIN_FRAME_COUNT - 1)`.
* 수정 사항 요약
    * 리소스 배열화: 프레임마다 독립적으로 사용해야 하는 리소스들을 배열로 선언.
    * Fence 관리: 각 프레임 컨텍스트마다 마지막 Fence 값을 추적하여 동기화.
    * Round-Robin: 프레임 인덱스를 순환하며 리소스를 사용 및 재활용.

### 코드 구현

* 프레임별로 독립적인 Command Allocator, Command List, Descriptor Pool 등을 배열로 관리합니다.

```cpp
const UINT SWAP_CHAIN_FRAME_COUNT = 3;
// CPU는 최대 2프레임까지 미리 명령을 쌓을 수 있음
const UINT MAX_PENDING_FRAME_COUNT = SWAP_CHAIN_FRAME_COUNT - 1;

class CD3D12Renderer
{
    // ... (기존 멤버 변수들)

    // 프레임별 리소스 (배열로 변경)
    ID3D12CommandAllocator*     m_ppCommandAllocator[MAX_PENDING_FRAME_COUNT] = {};
    ID3D12GraphicsCommandList*  m_ppCommandList[MAX_PENDING_FRAME_COUNT] = {};
    CDescriptorPool*            m_ppDescriptorPool[MAX_PENDING_FRAME_COUNT] = {};
    CSimpleConstantBufferPool*  m_ppConstantBufferPool[MAX_PENDING_FRAME_COUNT] = {};

    // 동기화 객체
    UINT64  m_pui64LastFenceValue[MAX_PENDING_FRAME_COUNT] = {}; // 각 컨텍스트의 마지막 Fence 값
    UINT64  m_ui64FenceVaule = 0;                                // 전역 Fence 카운터
    
    // 현재 사용 중인 리소스 인덱스 (0 ~ MAX_PENDING_FRAME_COUNT-1)
    DWORD   m_dwCurContextIndex = 0; 

    // ...
};
```

```cpp
// Command Allocator & List 생성
void CD3D12Renderer::CreateCommandList()
{
    for (DWORD i = 0; i < MAX_PENDING_FRAME_COUNT; i++)
    {
        m_pD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_ppCommandAllocator[i]));
        m_pD3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_ppCommandAllocator[i], nullptr, IID_PPV_ARGS(&m_ppCommandList[i]));
        m_ppCommandList[i]->Close(); // 생성 직후에는 닫아둠
    }
}

// Descriptor Pool 및 Constant Buffer Pool 초기화
// (Initialize 함수 내부)
for (DWORD i = 0; i < MAX_PENDING_FRAME_COUNT; i++)
{
    m_ppDescriptorPool[i] = new CDescriptorPool;
    m_ppDescriptorPool[i]->Initialize(m_pD3DDevice, MAX_DRAW_COUNT_PER_FRAME * ...);

    m_ppConstantBufferPool[i] = new CSimpleConstantBufferPool;
    m_ppConstantBufferPool[i]->Initialize(m_pD3DDevice, ...);
}
```

```cpp
void CD3D12Renderer::Present()
{
    // 1. 현재 프레임의 작업이 끝났음을 표시하는 Fence Signal 삽입
    Fence();

    // 2. 화면 출력 (Swap Chain Flip)
    // DXGI_PRESENT_ALLOW_TEARING 옵션 등 사용 가능
    m_pSwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
    m_uiRenderTargetIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    // 3. 다음 프레임 준비 (Context Index 순환)
    DWORD dwNextContextIndex = (m_dwCurContextIndex + 1) % MAX_PENDING_FRAME_COUNT;

    // ★ 핵심: 다음 사용할 컨텍스트가 GPU에서 작업이 완료되었는지 확인
    // 즉, 현재 프레임이 아닌 '이전 사이클의 해당 인덱스 프레임'을 기다림
    WaitForFenceValue(m_pui64LastFenceValue[dwNextContextIndex]);

    // 4. 다음 프레임을 위해 리소스 리셋 (GPU 사용이 끝났으므로 안전)
    m_ppConstantBufferPool[dwNextContextIndex]->Reset();
    m_ppDescriptorPool[dwNextContextIndex]->Reset();
    
    // 인덱스 교체
    m_dwCurContextIndex = dwNextContextIndex;
}

UINT64 CD3D12Renderer::Fence()
{
    m_ui64FenceVaule++;
    m_pCommandQueue->Signal(m_pFence, m_ui64FenceVaule);
    // 현재 컨텍스트가 어느 Fence 값까지 작업을 예약했는지 기록
    m_pui64LastFenceValue[m_dwCurContextIndex] = m_ui64FenceVaule;
    return m_ui64FenceVaule;
}
```

```cpp
void CD3D12Renderer::WaitForAllFrames()
{
    // 현재 쌓여있는 모든 명령 실행 보장
    Fence(); 
    
    // 모든 슬롯의 작업이 끝날 때까지 대기
    for (DWORD i = 0; i < MAX_PENDING_FRAME_COUNT; i++)
    {
        WaitForFenceValue(m_pui64LastFenceValue[i]);
    }
}

// 예: 텍스처 삭제 시
void CD3D12Renderer::DeleteTexture(void* pHandle)
{
    WaitForAllFrames(); // ★ 사용 중인 리소스를 삭제하면 안 되므로 전체 대기

    // 안전하게 리소스 해제
    // ...
}
```