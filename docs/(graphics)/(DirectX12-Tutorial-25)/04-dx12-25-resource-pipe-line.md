---
layout: default
title: "04. Resource Create–Update–Draw 파이프라인 요약"
parent: "(DirectX12 25년 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 1. GPU Buffer 생성 (Default Heap)

```cpp
device->CreateCommittedResource(
    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
    ...
    D3D12_RESOURCE_STATE_COPY_DEST,
    ...
);
```

* 메모리 위치
    * GPU VRAM(Default Heap)
    * CPU 직접 접근 불가.
    * 이후 Draw 시 Vertex Buffer 또는 Index Buffer 등으로 사용함.
    * 초기 상태를 COPY_DEST로 설정 → 나중에 UploadBuffer에서 복사받을 준비.


## 2. Upload Buffer 생성 (Upload Heap)

```cpp
device->CreateCommittedResource(
    &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
    ...
    D3D12_RESOURCE_STATE_GENERIC_READ,
    ...
);
```

* 메모리 위치
    * CPU RAM(Upload Heap)
    * CPU가 Map()으로 직접 쓰기 가능.
    * GPU는 이 메모리를 느린 경로로 읽을 수 있음 → 일시적인 데이터 전송용.

```
CPU writes → [Upload Heap (RAM)] → Copy → [Default Heap (VRAM)]
```

## 3. GPU Buffer를 Copy Dest 상태로 전환

```cpp
cmdList->ResourceBarrier(... COPY_DEST ...);
```

* 이미 COPY_DEST로 생성했다면 생략 가능.
* GPU Buffer가 “업데이트받을 준비 완료” 상태로 전환됨.

## 4. Upload → GPU Buffer 복사

```cpp
cmdList->CopyResource(gpuBuffer, uploadBuffer);
```

* GPU의 copy 엔진이 작동하여 VRAM으로 데이터를 이동.
* UploadBuffer는 이후 필요 없다면 재사용하거나 해제 가능.

## 5. GPU Buffer를 Draw 가능한 상태(Vertex Buffer)로 전환

```cpp
cmdList->ResourceBarrier(... VERTEX_AND_CONSTANT_BUFFER ...);
```

* Copy가 끝났으니 이제 그래픽 파이프라인의 IA(Input Assembler) 단계에서 읽기 가능하도록 상태 변경

## 6. Draw 호출

```cpp
cmdList->IASetVertexBuffers(...);
cmdList->DrawInstanced(...);
```

* Vertex Buffer View → GPU VRAM(Default Heap)
* GPU 쉐이더가 VRAM의 정점 데이터를 직접 읽어 렌더링.

## 7. Execute 명령 제출

```cpp
cmdQueue->ExecuteCommandLists(...);
```

## 전체 흐름

```
   CPU                          GPU
 ─────────                  ──────────
 1. Create Upload Heap (RAM)
        │
 2. Write vertex data via Map()
        │  CopyResource
        ▼
 [Upload Heap (RAM)] ───────────────► [Default Heap (VRAM)]
                                     (GPU 전용 Buffer)

 3. State transition: COPY_DEST → VERTEX_BUFFER
        │
 4. IASetVertexBuffers()
        │
 5. Draw()
        ▼
  GPU 파이프라인에서 정점 데이터(VRAM) 사용

```

---

## 전체적 코드를 보자

```cpp
HRESULT CD3D12ResourceManager::CreateVertexBuffer(UINT SizePerVertex, DWORD dwVertexNum, D3D12_VERTEX_BUFFER_VIEW* pOutVertexBufferView, ID3D12Resource **ppOutBuffer, void* pInitData)
{
	HRESULT hr = S_OK;

	D3D12_VERTEX_BUFFER_VIEW	VertexBufferView = {};
	ID3D12Resource*	pVertexBuffer = nullptr;
	ID3D12Resource*	pUploadBuffer = nullptr;
	UINT		VertexBufferSize = SizePerVertex * dwVertexNum;

	// create vertexbuffer for rendering
	hr = m_pD3DDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(VertexBufferSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&pVertexBuffer));

	if (FAILED(hr))
	{
		__debugbreak();
		goto lb_return;
	}
	if (pInitData)
	{
		if (FAILED(m_pCommandAllocator->Reset()))
			__debugbreak();

		if (FAILED(m_pCommandList->Reset(m_pCommandAllocator, nullptr)))
			__debugbreak();

		hr = m_pD3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(VertexBufferSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&pUploadBuffer));

		if (FAILED(hr))
		{
			__debugbreak();
			goto lb_return;
		}
		
		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin = nullptr;
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.

		hr = pUploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		if (FAILED(hr))
		{
			__debugbreak();
			goto lb_return;
		}
		memcpy(pVertexDataBegin, pInitData, VertexBufferSize);
		pUploadBuffer->Unmap(0, nullptr);

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pVertexBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
		m_pCommandList->CopyBufferRegion(pVertexBuffer, 0, pUploadBuffer, 0, VertexBufferSize);
		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		m_pCommandList->Close();

		ID3D12CommandList* ppCommandLists[] = { m_pCommandList };
		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		
		Fence();
		WaitForFenceValue();
	}
	

	// Initialize the vertex buffer view.
	VertexBufferView.BufferLocation = pVertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = SizePerVertex;
	VertexBufferView.SizeInBytes = VertexBufferSize;

	*pOutVertexBufferView = VertexBufferView;
	*ppOutBuffer = pVertexBuffer;

lb_return:
	if (pUploadBuffer)
	{
		pUploadBuffer->Release();
		pUploadBuffer = nullptr;
	}
	return hr;
}
```