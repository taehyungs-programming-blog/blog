---
layout: default
title: "02. 기초적 이론 재정리"
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

> 기억이 나지 않는 부분이 있어서 재 정리.

---

## C++에서 HLSL로 연결되는 코스

### 전체 흐름 개요

1. 리소스(Resource) 생성
2. Descriptor Heap 생성
3. View(Descriptor) 생성 및 Heap에 기록
4. Root Signature에서 Descriptor Table 정의
5. Command List에서 Descriptor Table 바인딩
6. Shader에서 register(t0/b0) 사용

### 1. 리소스(Resource) 생성

```cpp
// 텍스처와 상수 버퍼 리소스를 생성한다고 가정합니다.
ComPtr<ID3D12Resource> gTexture;        // GPU 텍스처 리소스 (VRAM)
ComPtr<ID3D12Resource> gConstantBuffer; // GPU 상수 버퍼 리소스

// ... (리소스는 일반적으로 CreateCommittedResource()로 생성)
// 예시: 상수 버퍼 생성
D3D12_HEAP_PROPERTIES heapProps = { D3D12_HEAP_TYPE_UPLOAD };
D3D12_RESOURCE_DESC cbDesc = CD3DX12_RESOURCE_DESC::Buffer(256);
device->CreateCommittedResource(
    &heapProps, D3D12_HEAP_FLAG_NONE, &cbDesc,
    D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
    IID_PPV_ARGS(&gConstantBuffer)
);

```

### 2. Descriptor Heap 생성

```cpp
ComPtr<ID3D12DescriptorHeap> gHeap;

D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
heapDesc.NumDescriptors = 2; // CBV 1개 + SRV 1개
heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&gHeap));
```

* “Descriptor들을 담을 GPU-visible 배열”을 하나 만든 상태라고 보면 된다.

```
[ Descriptor Heap (CBV_SRV_UAV) ]
+--------+--------+
| Slot 0 | Slot 1 |
+--------+--------+
  (비어있음)

```

### 3. View(Descriptor) 생성 → Heap에 기록

```cpp
// Slot 0: CBV (상수 버퍼)
D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
cbvDesc.BufferLocation = gConstantBuffer->GetGPUVirtualAddress();
cbvDesc.SizeInBytes = 256;

auto cpuHandle = gHeap->GetCPUDescriptorHandleForHeapStart();
device->CreateConstantBufferView(&cbvDesc, cpuHandle);

// Slot 1: SRV (텍스처)
D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
srvDesc.Texture2D.MipLevels = 1;

UINT descSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
cpuHandle.ptr += descSize; // 다음 슬롯으로 이동
device->CreateShaderResourceView(gTexture.Get(), &srvDesc, cpuHandle);

```

* 결과적으로 Heap 상태는 이렇게 됨

```
[ Descriptor Heap (CBV_SRV_UAV) ]
+--------------------------------------------------+
| Slot 0 → CBV(리소스=gConstantBuffer, register=b0) |
| Slot 1 → SRV(리소스=gTexture, register=t0)        |
+--------------------------------------------------+
```

### 4. Root Signature (Descriptor Table 정의)

```cpp
// 두 Range 정의: SRV(t0) 1개, CBV(b0) 1개
D3D12_DESCRIPTOR_RANGE ranges[2] = {};
ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
ranges[0].NumDescriptors = 1;
ranges[0].BaseShaderRegister = 0; // t0
ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
ranges[1].NumDescriptors = 1;
ranges[1].BaseShaderRegister = 0; // b0

// Descriptor Table 하나 생성
D3D12_ROOT_PARAMETER rootParam = {};
rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
rootParam.DescriptorTable.NumDescriptorRanges = 2;
rootParam.DescriptorTable.pDescriptorRanges = ranges;
rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

// Root Signature 생성
D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
rootSigDesc.NumParameters = 1;
rootSigDesc.pParameters = &rootParam;
rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

ComPtr<ID3DBlob> blob, error;
D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
ComPtr<ID3D12RootSignature> gRootSig;
device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
    IID_PPV_ARGS(&gRootSig));

```

* Root Signature는 GPU에게 말합니다:
    * “이 셰이더는 SRV(t0) 1개와 CBV(b0) 1개를 하나의 Descriptor Table로 사용할 거야.

### 5. Descriptor Table을 셰이더에 바인딩

```cpp
cmdList->SetGraphicsRootSignature(gRootSig.Get());

// Descriptor Heap 설정
ID3D12DescriptorHeap* heaps[] = { gHeap.Get() };
cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

// Root Parameter 0번에 Descriptor Table 바인딩
D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = gHeap->GetGPUDescriptorHandleForHeapStart();
cmdList->SetGraphicsRootDescriptorTable(0, gpuHandle);

```

* 이제 셰이더에서 register(t0)를 참조하면 Heap의 Slot 1에 있는 SRV로,
    * register(b0)를 참조하면 Slot 0의 CBV로 연결
* 즉, Root Signature의 “지도”에 따라 Descriptor Table과 Heap이 연결된 상태입니다.

### 6. 셰이더 (HLSL)

```cpp
Texture2D    gTexture : register(t0);
SamplerState gSampler : register(s0);
cbuffer PerObject : register(b0)
{
    float4 color;
};

float4 PSMain(float2 uv : TEXCOORD) : SV_Target
{
    float4 tex = gTexture.Sample(gSampler, uv);
    return tex * color;
}

```

---

## 전체 흐름

```
┌──────────────────────────┐
│ GPU Resource (Texture)   │  ← 실제 데이터 (VRAM)
└──────────────────────────┘
            │
            ▼
┌──────────────────────────┐
│ Descriptor (SRV)         │  ← "이 Texture는 t0로 접근, 2D, format=RGBA8"
└──────────────────────────┘
            │
            ▼
┌──────────────────────────┐
│ Descriptor Heap          │
│  Slot 0 → CBV(b0)        │
│  Slot 1 → SRV(t0)        │
└──────────────────────────┘
            │
            ▼
┌──────────────────────────┐
│ Descriptor Table         │
│  Range0: t0 (SRV 1개)    │
│  Range1: b0 (CBV 1개)    │
└──────────────────────────┘
            │
            ▼
┌──────────────────────────┐
│ Root Signature           │
│  Param0 = DescriptorTable│
│  (t0,b0) 정의             │
└──────────────────────────┘
            │
            ▼
┌──────────────────────────┐
│ Shader (HLSL)            │
│  uses register(t0,b0)    │
└──────────────────────────┘

```

---

# D3D12 Command List / Command Queue 정리 (예시 코드 포함)

## 1. 개념 요약

### 1.1 Command List

- **역할**: GPU에 보낼 명령(`Draw`, `Dispatch`, `Copy`, `ResourceBarrier` 등)을 **CPU가 미리 기록**해 두는 객체  
- **수명**:
  1. `Reset()` → 기록 시작
  2. 각종 명령 기록
  3. `Close()` → 기록 종료
  4. Command Queue에 제출(Execute)
- **Command Allocator** 위에 명령을 쓰는 “뷰” 같은 존재이며, 실제 메모리는 Allocator가 가짐

### 1.2 Command Queue

- **역할**: Close된 Command List들을 **GPU에 제출하는 큐**
- **특징**:
  - `ExecuteCommandLists()`를 통해 여러 Command List를 한 번에 제출 가능
  - GPU는 큐에 들어온 순서대로 실행
  - Fence를 이용해 CPU와 GPU 간 동기화를 수행

### 1.3 타입

- 공통 타입 (List / Queue 모두 동일 타입 사용):
  - `D3D12_COMMAND_LIST_TYPE_DIRECT` : 일반적인 그래픽스 + 복사 작업
  - `D3D12_COMMAND_LIST_TYPE_COMPUTE`: Compute 전용
  - `D3D12_COMMAND_LIST_TYPE_COPY`   : Copy 전용 (업로드/다운로드, 스트리밍 등)

---

## 2. 기본 초기화 예시

아래 코드는 **C++14 / WRL::ComPtr** 기준의 단순 예시입니다.  
(에러 체크를 단순하게 하기 위해 `ThrowIfFailed` 헬퍼를 사용합니다.)

```cpp
// d3d12_example.h

#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <stdexcept>

using Microsoft::WRL::ComPtr;

inline void ThrowIfFailed(HRESULT hr) {
  if (FAILED(hr)) {
    throw std::runtime_error("D3D12 call failed");
  }
}

struct D3DContext {
  ComPtr<IDXGIFactory6> factory;
  ComPtr<ID3D12Device> device;

  // Queue
  ComPtr<ID3D12CommandQueue> direct_queue;

  // Allocator / List (단일 프레임 예시)
  ComPtr<ID3D12CommandAllocator> command_allocator;
  ComPtr<ID3D12GraphicsCommandList> command_list;

  // Fence
  ComPtr<ID3D12Fence> fence;
  HANDLE fence_event = nullptr;
  UINT64 fence_value = 0;
};
```

```cpp
// d3d12_example.cpp

#include "d3d12_example.h"

void InitD3D(D3DContext* ctx) {
  // 1. DXGI Factory 생성
  UINT dxgi_flags = 0;
#if defined(_DEBUG)
  {
    ComPtr<ID3D12Debug> debug_controller;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)))) {
      debug_controller->EnableDebugLayer();
      dxgi_flags |= DXGI_CREATE_FACTORY_DEBUG;
    }
  }
#endif

  ThrowIfFailed(CreateDXGIFactory2(dxgi_flags, IID_PPV_ARGS(&ctx->factory)));

  // 2. 하드웨어 어댑터 선택 (단순 예시: 첫 번째 사용 가능한 HW 어댑터)
  ComPtr<IDXGIAdapter1> adapter;
  for (UINT adapter_index = 0;
       ctx->factory->EnumAdapters1(adapter_index, &adapter) != DXGI_ERROR_NOT_FOUND;
       ++adapter_index) {
    DXGI_ADAPTER_DESC1 desc = {};
    adapter->GetDesc1(&desc);

    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
      // WARP 어댑터는 여기서 스킵
      continue;
    }

    if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                    IID_PPV_ARGS(&ctx->device)))) {
      break;
    }
  }

  // 3. Command Queue 생성 (DIRECT 타입)
  D3D12_COMMAND_QUEUE_DESC queue_desc = {};
  queue_desc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;
  queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

  ThrowIfFailed(ctx->device->CreateCommandQueue(
      &queue_desc, IID_PPV_ARGS(&ctx->direct_queue)));

  // 4. Command Allocator / Command List 생성
  ThrowIfFailed(ctx->device->CreateCommandAllocator(
      D3D12_COMMAND_LIST_TYPE_DIRECT,
      IID_PPV_ARGS(&ctx->command_allocator)));

  ThrowIfFailed(ctx->device->CreateCommandList(
      0,
      D3D12_COMMAND_LIST_TYPE_DIRECT,
      ctx->command_allocator.Get(),
      /*pInitialState=*/nullptr,
      IID_PPV_ARGS(&ctx->command_list)));

  // Command List는 생성 직후 open 상태이므로, 우선 Close해 둠
  ThrowIfFailed(ctx->command_list->Close());

  // 5. Fence 생성 및 이벤트 생성
  ThrowIfFailed(ctx->device->CreateFence(
      0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&ctx->fence)));
  ctx->fence_value = 1;

  ctx->fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
  if (ctx->fence_event == nullptr) {
    ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
  }
}

```