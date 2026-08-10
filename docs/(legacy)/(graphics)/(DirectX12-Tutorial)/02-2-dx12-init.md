---
layout: default
title: "02-02. 장치 초기화 (CommandQueue)"
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

## CommandQueue

* DirectX는 GPU에 실시간으로 명령을 보내는 것이 아니라 **Queue를 통해서 GPU에게 명령을 요청해 두고 GPU는 queue에서 명령을 꺼내서 처리**하는 형식이다.

* 어떻게 명령을 보내는지 간략하게 설명하자면
* 아래 세 녀석이 쓰인다.

```cpp
// 총 대장 명령을 보낼지 말지 결정함.
ComPtr<ID3D12CommandQueue>			_cmdQueue;

// ** 이런식으로 쓰인다 **
    // 생성 (Device가 생성해 줘야함.)
device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));

    // fence에 Event보내기
_cmdQueue->Signal(_fence.Get(), _fenceValue);

    // 명령 보내기
_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);
```

```cpp
// 일감이 쌓이는 메모리공간
ComPtr<ID3D12CommandAllocator>		_cmdAlloc;

// ** 이런식으로 쓰인다 **
    // 생성 (Device가 생성해 줘야함.)
device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));
    
    // 명령 쌓기
void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
    // 비우고
	_cmdAlloc->Reset();
    // 여기를 쓰라고 List에게 알려줌.
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);
```

```cpp
// 일감 리스트 목록 (사실상 Worker)
ComPtr<ID3D12GraphicsCommandList>	_cmdList;

// ** 이런식으로 쓰인다 **
    // 생성
device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));

    // 할일 쌓기
_cmdList->ResourceBarrier(1, &barrier);

// Viewport와 Scissor 정의
_cmdList->RSSetViewports(1, vp);
_cmdList->RSSetScissorRects(1, rect);

// RenderTarget 정의
D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _descHeap->GetBackBufferView();
_cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr);
_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, nullptr);

    // 할일 보내기
_cmdList->ResourceBarrier(1, &barrier);
_cmdList->Close();
```

### ResourceBarrier에 관해서

* DirectX 12에서 ResourceBarrier 함수는 **GPU 리소스의 상태를 변경하는 데 사용**됩니다. 

```cpp
D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
    _swapChain->GetCurrentBackBufferResource().Get(),
    D3D12_RESOURCE_STATE_PRESENT, // 화면 출력
    D3D12_RESOURCE_STATE_RENDER_TARGET); // 외주 결과물

_cmdList->ResourceBarrier(1, &barrier);
```

* **리소스 상태 관리**: GPU 리소스, 특히 텍스처나 버퍼와 같은 리소스의 사용 상태를 변경합니다. 예를 들어, 리소스를 렌더 타겟으로부터 셰이더 읽기용으로, 또는 그 반대로 전환할 수 있습니다.
* **동기화 및 성능 최적화**: 상태 전환은 GPU의 동기화 및 효율적인 리소스 사용을 위해 중요합니다. 올바른 상태로 리소스를 전환시키지 않으면, GPU는 예상치 못한 동작을 보일 수 있으며, 성능 저하의 원인이 될 수도 있습니다.

```cpp
// 예를 들어, 특정 텍스처가 렌더링 타겟으로 사용된 후 셰이더에서 읽기용으로 사용되어야 한다면, 
// 렌더링 작업이 끝난 후 해당 텍스처의 상태를 D3D12_RESOURCE_STATE_RENDER_TARGET에서 D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE로 전환해야 합니다. 
// 이 상태 전환은 ResourceBarrier 함수를 통해 수행됩니다.

D3D12_RESOURCE_BARRIER barrier = {};
barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
barrier.Transition.pResource = texture;
barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

commandList->ResourceBarrier(1, &barrier);
```

---

## 구현된 코드를 보자

```cpp
class CommandQueue
{
	// ...

private:
	// CommandQueue : DX12에 등장
	// 외주를 요청할 때, 하나씩 요청하면 비효율적
	// [외주 목록]에 일감을 차곡차곡 기록했다가 한 방에 요청하는 것

	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	// 일감을 보내는 녀석
	// (참고) _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	ComPtr<ID3D12CommandAllocator>		_cmdAlloc;
	// 일감이 쌓이는 메모리공간

	ComPtr<ID3D12GraphicsCommandList>	_cmdList;
	// 일감 리스트 목록

	// Fence : 울타리(?)
	// CPU / GPU 동기화를 위한 간단한 도구, 아직은 안쓰이기에 일단은 생략
	// CPU에서 외주준 데이터가 GPU가 작업완료 될때까지 대기한다.
	ComPtr<ID3D12Fence>					_fence;
	uint32								_fenceValue = 0;		// fence번호(CPU가 외주한 데이터가 몇번인지 기록)
	// 일감에 숫자를 지정해 자신이 명령한 일감이 맞는지 확인용
	HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;
	// Event를 통해 GPU의 작업이 완료됐는지 통보 받을수 있음.

	// ...
};
```

```cpp
void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect) 
{
	// 다시 명령을 보낼경우 alloc된 메모리와 list를 초기화 해줘야하며,
	// list에게는 다시 alloc된 메모리 공간정보를 알려줘야한다.
	_cmdAlloc->Reset(); 
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);
	
	// ...
```

* `CommandQueue::RenderBegin` : 명령을 list에 담는다

```cpp
void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect) 
{
	// 명령 메모리 공간할당
	_cmdAlloc->Reset();

	// 명령을 담을 List를 준비
	_cmdList->Reset(_cmdAlloc.Get(), nullptr); 


    // 여긴 SwapChain을 먼저보고 오시오.
	// barrier(장벽)을 두고 특정메모리 공간을 어떻게 쓸것인가를 알린다.
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( 
		_swapChain->GetCurrentBackBufferResource().Get(), 
		D3D12_RESOURCE_STATE_PRESENT, 		 // (before) 현재 출력중인 화면 
		D3D12_RESOURCE_STATE_RENDER_TARGET); // (after) GPU가 작업중인 공간 
		// before(기존에)는 현재 출력중인 화면이였는데 after(지금부터)는 GPU가 작업할 공간으로 쓰자.
        // 메모리 공간 _swapChain->GetCurrentBackBufferResource().Get()을
        // 현재 D3D12_RESOURCE_STATE_PRESENT로 쓰고 있는데
        // D3D12_RESOURCE_STATE_RENDER_TARGET로 쓰게 해달라

        // (참고) ComPtr<ID3D12Resource> GetCurrentBackBufferResource() { return _renderTargets[_backBufferIndex]; }
            // (참고) ComPtr<ID3D12Resource>	_renderTargets[SWAP_CHAIN_BUFFER_COUNT];
            // _renderTargets는 ID3D12Resource임


	// 위 화면교체 명령을 바로 실행하는 것이 아니라 List에 추가 해 놓는 형태이다. 
	_cmdList->ResourceBarrier(1, &barrier);

	// Viewport, ScissorRect 설정(위에 참고)
	_cmdList->RSSetViewports(1, vp); 
	_cmdList->RSSetScissorRects(1, rect);

	// Specify the buffers we are going to render to. 
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _descHeap->GetBackBufferView(); 
	_cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr); 
	_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, nullptr);
	// GPU에게 backBufferView 백버퍼에 그려달라고 요청
	// OMSetRenderTargets : Render-Target과 Depth-Stencil-View의 Merger (Output-Merger(OM)) (필수라 넣음)
	    // Render-Target과 Depth-Stencil-View를 합쳐주는 부분인데 일단은 받아 들인다.

	// 어쨋든 중요한건 _cmdList에 명령을 담고 있는것을 주목해서 보자.
```

* `CommandQueue::RenderEnd` : queue를 통해서 명령을 보낸다.

```cpp
void CommandQueue::RenderEnd() 
{ 
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( 
		_swapChain->GetCurrentBackBufferResource().Get(), 
		D3D12_RESOURCE_STATE_RENDER_TARGET, 	// (before) GPU가 작업중인 공간 
		D3D12_RESOURCE_STATE_PRESENT); 			// (after) 화면 출력중인 화면 


	// List에 명령을 넣는다. 
	_cmdList->ResourceBarrier(1, &barrier); 
    _cmdList->Close();
	// 이제 더이상 보낼 명령이 없다고 알림 

	// 커맨드 리스트 수행 (여기 전 까진 리스트에 넣기만 하지 명령을 수행하진 않음.)
	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() }; 
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);
	// 실행해 주세요 
	// 일감을 보낸다.

	_swapChain->Present(); 
	// Wait until frame commands are complete.  This waiting is inefficient and is 
	// done for simplicity.  Later we will show how to organize our rendering code 
	// so we do not have to wait per frame. 
	
    // Fence를 통해 Wait함수내에서 대기
    WaitSync(); 

	_swapChain->SwapIndex(); 
}
```

