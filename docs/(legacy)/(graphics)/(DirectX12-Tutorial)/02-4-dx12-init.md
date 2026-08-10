---
layout: default
title: "02-04. 장치 초기화 (DescriptorHeap)"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## DescriptorHeap(Render-Target View로 현재는 쓰임)

* DirectX 12에서 Descriptor Heap은 **GPU 리소스에 대한 참조 또는 핸들을 저장**하는 데 사용되는 객체입니다. 
* 디스크립터 힙은 **그래픽 파이프라인에 필요한 다양한 리소스를 GPU에 전달**하는 중심적인 메커니즘입니다. 

* 디스크립터 힙의 주요 기능과 특징
    * **리소스 참조 저장**: 디스크립터 힙은 GPU 리소스(예: 텍스처, 버퍼, 샘플러)에 대한 참조를 저장합니다. 이 참조는 '디스크립터'라고 불리며, GPU가 필요한 리소스를 식별하고 접근할 수 있게 해줍니다.

* 유형: DirectX 12에서는 여러 유형의 디스크립터 힙이 있습니다:
    * **CBV/SRV/UAV (Constant Buffer View / Shader Resource View / Unordered Access View) 힙**: 셰이더가 사용하는 버퍼와 텍스처에 대한 참조를 저장합니다.
    * **Sampler 힙**: 텍스처 샘플링에 사용되는 샘플러 상태를 저장합니다.
    * **RTV (Render Target View) 및 DSV (Depth Stencil View) 힙**: 렌더링 타겟과 깊이/스텐실 버퍼에 대한 참조를 저장합니다.
* 효율적인 리소스 관리: 디스크립터 힙을 사용함으로써, 개발자는 GPU 리소스를 효율적으로 관리할 수 있습니다. 예를 들어, 여러 셰이더에서 공통으로 사용되는 리소스를 디스크립터 힙에 한 번만 할당하고 필요할 때마다 참조할 수 있습니다.

* descriptorheap 이론적 설명 -> [MSDN 🌎](https://learn.microsoft.com/ko-kr/windows/win32/direct3d12/descriptor-heaps-overview)

```cpp
void DescriptorHeap::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{
	_swapChain = swapChain;

	// Descriptor (DX12) = View (~DX11)
	// [서술자 힙]으로 RTV 생성
	// DX11의 RTV(RenderTargetView), DSV(DepthStencilView), 
	// CBV(ConstantBufferView), SRV(ShaderResourceView), UAV(UnorderedAccessView)
        // 를 전부 DescriptorHeap에서 관리

	_rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// RTV에 대한 설명
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDesc.NodeMask = 0;

	// 같은 종류의 데이터끼리 배열로 관리
		// RenderTarget은 Back, Front가 있으니 두 개가 있겠지?
	// RTV 목록 : [ ] [ ]
	// RTV를 배열로 생성해주세요
	device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&_rtvHeap));

	// <<< 위는 Descriptor Heap 생성
	// >>> 아래는 Descriptor Heap 채우기

	// RTV의 주소값을 줘
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();

	for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
	{
		_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * _rtvHeapSize);
		// SwapChain에 있는 RTV 주소를 가져와 생성
		device->CreateRenderTargetView(swapChain->GetRenderTarget(i).Get(), nullptr, _rtvHandle[i]);
	}
}

// 아직은 DescriptorHeap을 생성만 했기에 어떻게 사용될지 감이 안온다
// 이후에 이 부분은 설명예정
```

```cpp
D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetBackBufferView()
{
    return GetRTV(_swapChain->GetCurrentBackBufferIndex());
/*
	D3D12_CPU_DESCRIPTOR_HANDLE		GetRTV(int32 idx) { return _rtvHandle[idx]; }
*/
}
```

* 대략 이렇게 쓰이는데, RenderTarget 를 직접 GPU에 전달하는게 아니라
* RenderTarget의 View 여기서는 Descriptor를 GPU에 전달하는 개념이라 생각하면된다.

---

```cpp
void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
	// CommandQuque를 비워주고
    _cmdAlloc->Reset();
    _cmdList->Reset(_cmdAlloc.Get(), nullptr);


    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _swapChain->GetCurrentBackBufferResource().Get(),
        D3D12_RESOURCE_STATE_PRESENT,         	// <- Before
        D3D12_RESOURCE_STATE_RENDER_TARGET); 	// <- After
	// Before(기존)에는 D3D12_RESOURCE_STATE_PRESENT(출력중) 상태였는데
	// After(앞으로)는 D3D12_RESOURCE_STATE_RENDER_TARGET(그리는중) 으로 변경해 달라

    // 위 명령을 commandqueue에 넣기
    _cmdList->ResourceBarrier(1, &barrier);

    // commandqueue를 사용하면 아래를 해줘야함.
    // Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
    _cmdList->RSSetViewports(1, vp);
    _cmdList->RSSetScissorRects(1, rect);


    // backbuffer에 그림을 그려달라고 명령
    // Specify the buffers we are going to render to.
    D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _descHeap->GetBackBufferView();
    _cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr);
    _cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, nullptr);
}



void CommandQueue::RenderEnd()
{
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _swapChain->GetCurrentBackBufferResource().Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, 	// Before
        D3D12_RESOURCE_STATE_PRESENT); 			// After
    // RenderStart와 반대임을 알수 있다.


    _cmdList->ResourceBarrier(1, &barrier);
    _cmdList->Close();


    // 커맨드 리스트 수행
    ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    _swapChain->Present();


    // Wait until frame commands are complete.  This waiting is inefficient and is
    // done for simplicity.  Later we will show how to organize our rendering code
    // so we do not have to wait per frame.
    WaitSync();
    _swapChain->SwapIndex();
}
```