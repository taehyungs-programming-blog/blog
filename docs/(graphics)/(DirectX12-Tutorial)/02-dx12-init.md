---
layout: default
title: "02. 장치 초기화"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/2)

## 들어가며...

* DirectX를 조금 편하게 사용하기 위해서 아래 다섯 클래스를 정의할 예정이다.

1. Engine
2. Device
3. CommandQueue
4. SwapChain
5. DesciptorHeap

* DirectX를 사용해봤다면 이름만 봐도 대충 느낌이 오겠지만... 초보자라면 일단은 받아들여야 할 부분이 많을 것이다. ㅎㅎ

---

## Engine

* Engine이 하는일은?
    * 위에서 언급된 4개의 클래스(Device, CommandQueue, SwapChain, DesciptorHeap)를 생성/초기화
    * 그려질 Rect정보를 갖고있으며
    * Render정보를 queue에 전달

```cpp
#pragma once

class Engine
{
public:

	// 초기화
	void Init(const WindowInfo& info);
	/*
	struct WindowInfo
	{
		HWND hwnd;		// 핸들
		int32 width
		int32 height;
		bool windowed;	// 창모드
	};
	*/

	// 렌더링
	void Render();

public:
	void RenderBegin();
	void RenderEnd();

	void ResizeWindow(int32 width, int32 height);

private:
	WindowInfo		_window;
	// 위도우 정보(크기, 핸들 등을 담는다)

	D3D12_VIEWPORT	_viewport = {};
	// Describes the dimensions of a viewport.
	// x, y, width, height 를 정의
	// _viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };
	
	D3D12_RECT		_scissorRect = {};
	// Just Rect
	// 역시 x, y, width, height 를 정의
	// 단, 랜더링을 어디서 부터할지를 scissorRect에서 정의한다.
	// _scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

	// 아래 네 가지 클래스로 DirectX를 컨트롤하게 되며
	// 네 클래스를 관리하는 것이 Engine 클래스 이다.
	shared_ptr<class Device>         _device;
	shared_ptr<class CommandQueue>   _cmdQueue;
    shared_ptr<class SwapChain>      _swapChain;
	shared_ptr<class DescriptorHeap> _descHeap;
};
```

* viewport와 scissor rect 차이?
    * viewport : 렌더링을 할 렌더타겟(후면버퍼) 영역을 나타낸다.
    * scissor rect : 렌더링에서 제거하지 않을 영역을 설정한다. ScissorRect에 포함되지 않는 영역은 렌더링(레스터라이저)에서 제거된다.
        * (참고) 레스터라이저 단계(rasterizer stage) : 래스터화 단계는 실제로 화면에 그려지는 과정이다. 래스터화 단계에서는 3D 좌표계에서 2D 좌표계로 변환되는 과정이 이루어지며, 이 과정에서 렌더링을 제거할 영역을 설정할 수 있다.

```cpp
// ...

void Engine::Init(const WindowInfo& info)
{
	_window = info;
	ResizeWindow(info.width, info.height);

	// 그려질 화면 크기를 설정
	_viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

	// 모든 클래스를 여기서 초기화 하며 필요한 정보를 넘긴다.
	_device = make_shared<Device>();
	_cmdQueue = make_shared<CommandQueue>();
	_swapChain = make_shared<SwapChain>();
	_descHeap = make_shared<DescriptorHeap>();

	// 상세한 사항은 아래서 참조
        // 여기선 클래스 생성 및 초기화를 한다고 생각하자
	_device->Init();
	_cmdQueue->Init(_device->GetDevice(), _swapChain, _descHeap);
	_swapChain->Init(info, _device->GetDXGI(), _cmdQueue->GetCmdQueue());
	_descHeap->Init(_device->GetDevice(), _swapChain);
}

// ...
```

```cpp
void Engine::RenderBegin()
{
	// width, height 정보를 queue로 넘긴다.
	_cmdQueue->RenderBegin(&_viewport, &_scissorRect);
}
```

* queue를 먼저 잠깐보자면

```cpp
void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect) 
{
	// ... 
	// 명령리스트(일단 받아들이자)에 Viewport, Scissor Rect를 넘겨서 어디까지 그려달라 명령한다.
	_cmdList->RSSetViewports(1, vp); 
	_cmdList->RSSetScissorRects(1, rect);
```

---

## Device

```cpp
#pragma once

// 인력 사무소
class Device
{
public:
	void Init();

	ComPtr<IDXGIFactory> GetDXGI() { return _dxgi; }
	ComPtr<ID3D12Device> GetDevice() { return _device; }

private:
	// ComPtr : DirectX(COM객체)를 사용하는 일종의 스마트 포인터라 이해하면 된다.

	ComPtr<ID3D12Debug>			_debugController;
	// 디버그 출력을 담당(크게 중요하지 않음 무시해도 됨.)

	ComPtr<IDXGIFactory>		_dxgi; 
	// 화면 관련 기능들, DirectX 공통기능 API라 생각
	// 전체화면, 사용가능한 그래픽카드 리스트 등
	// SwapChain제작도 담당

	ComPtr<ID3D12Device>		_device; 
	// 그래픽 카드 자체를 의미
	// 각종 객체 생성, GPU와 통신을 담당하는 녀석
};
```

```cpp
#include "pch.h"
#include "Device.h"

void Device::Init()
{
	// D3D12 디버그층 활성화
	// - VC++ 출력창에 상세한 디버깅 메시지 출력
	// - riid : 디바이스의 COM ID
	// - ppDevice : 생성된 장치가 매개변수에 설정
#ifdef _DEBUG
	::D3D12GetDebugInterface(IID_PPV_ARGS(&_debugController));
        // IID_PPV_ARGS - 입력된 object의 riid와 pointer를 전달해 준다.
	_debugController->EnableDebugLayer();
#endif

	// DXGI(DirectX Graphics Infrastructure)
	// Direct3D와 함께 쓰이는 API
	// - 전체 화면 모드 전환
	// - 지원되는 디스플레이 모드 열거 등
	// CreateDXGIFactory
	// - riid : 디바이스의 COM ID
	// - ppDevice : 생성된 장치가 매개변수에 설정
	::CreateDXGIFactory(IID_PPV_ARGS(&_dxgi));

	// CreateDevice
	// - 디스플레이 어댑터(그래픽 카드)를 나타내는 객체
	// - pAdapter : nullptr 지정하면 시스템 기본 디스플레이 어댑터
	// - MinimumFeatureLevel : 응용 프로그램이 요구하는 최소 기능 수준 (구닥다리 걸러낸다)
	// - riid : 디바이스의 COM ID
	// - ppDevice : 생성된 장치가 매개변수에 설정
	::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device));
}
```

* **dxgi?**
    * [참고 🌍](https://learn.microsoft.com/ko-kr/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi)
    * Microsoft DXGI(DirectX Graphics Infrastructure)
    * 주요역할 👉 Graphic Driver Enum, Present 관련
* Graphic Driver Enum
    * 말 그대로 Grphic Driver 정보를 열거한다.
* Present
    * Swapchain 만들기
    * FullScreen 전환
    * 창크기 전환 등

* 사실 Device가 뭘하는지 잘모르겠는데 그냥 dxgi, device를 생성하고 관리한다 정도만 알아도 된다.

---

## CommandQueue

* DirectX는 GPU에 실시간으로 명령을 보내는 것이 아니라 Queue를 통해서 GPU에게 명령을 요청해 두고 GPU는 queue에서 명령을 꺼내서 처리하는 형식이다.

* 어떻게 명령을 보내는지 간략하게 설명하자면

1. Alloc을 통해서 명령을 보낼 메모리 공간을 GPU에 할당한다 : `_cmdList->Reset(_cmdAlloc.Get(), nullptr); `
2. 명령을 list에 담는다 : `_cmdList->RSSetViewports(1, vp); `
3. queue를 통해서 명령을 보낸다. : `_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);`

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

---

## Swapchain

```cpp
class SwapChain
{
public:
	void Init(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	void Present();
	void SwapIndex();

	ComPtr<IDXGISwapChain> GetSwapChain() { return _swapChain; }
	ComPtr<ID3D12Resource> GetRenderTarget(int32 index) { return _renderTargets[index]; }

	uint32 GetCurrentBackBufferIndex() { return _backBufferIndex; }
	ComPtr<ID3D12Resource> GetCurrentBackBufferResource() { return _renderTargets[_backBufferIndex]; }

private:
	ComPtr<IDXGISwapChain>	_swapChain;
	ComPtr<ID3D12Resource>	_renderTargets[SWAP_CHAIN_BUFFER_COUNT];
	uint32					_backBufferIndex = 0;
};
```

```cpp
#include "pch.h"
#include "SwapChain.h"

void SwapChain::Init(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	// 이전에 만든 정보 날린다
	_swapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = static_cast<uint32>(info.width); // 버퍼의 해상도 너비
	sd.BufferDesc.Height = static_cast<uint32>(info.height); // 버퍼의 해상도 높이
	sd.BufferDesc.RefreshRate.Numerator = 60; // 화면 갱신 비율
	sd.BufferDesc.RefreshRate.Denominator = 1; // 화면 갱신 비율
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 버퍼의 디스플레이 형식
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1; // 멀티 샘플링 OFF
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼에 렌더링할 것 
	sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT; // 전면+후면 버퍼
	sd.OutputWindow = info.hwnd;		// Present를 호출시 이 핸들에 그려준다.
	sd.Windowed = info.windowed;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 전면 후면 버퍼 교체 시 이전 프레임 정보 버림
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// swapchain은 dxgi를 통해서 만든다.
	dxgi->CreateSwapChain(cmdQueue.Get(), &sd, &_swapChain);

	// 스왑체인의 메모리 주소를 _renderTargets에 할당한다.
	for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
		_swapChain->GetBuffer(i, IID_PPV_ARGS(&_renderTargets[i]));

	// 참고) ComPtr<ID3D12Resource>	_renderTargets[SWAP_CHAIN_BUFFER_COUNT];
}

// ...

void SwapChain::Present()
{
	// Present the frame.
	// 화면에 프리젠트(출력)해 달라
	_swapChain->Present(0, 0);
	// init에서 받은 핸들에 그리게 된다.
}
```

---

## DescriptorHeap(Render-Target View)

* Render-Target? 그냥 백버퍼라 생각하면 마음편하다.
* Descriptor 혹은 View라고 한다. 여기선 Render-Target Desciptor or View라 하는데 Render-Taget(백버퍼)가 어떻게 구성될지에 대한 설명서 라고 생각하자
* Render-Target에 명령을 넣고 싶다면 여기를 만지면 되는데 현재는 사용되는 부분이 없기에 우선 설명은 생략한다.

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

* descriptorheap 이론적 설명 -> [MSDN 🌎](https://learn.microsoft.com/ko-kr/windows/win32/direct3d12/descriptor-heaps-overview)
* 아직은 봐도 이해안됨 ...;;

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

---

## Tips) Viewport, ScissorRect 차이점 분석

* 이렇게만 하면 이해가 안되는데 가운데 삼각형을 하나 그리고 view, scissor를 테스트해보자

```cpp
// 기본화면

void Engine::Init(const WindowInfo& info)
{
	_window = info;
	ResizeWindow(info.width, info.height);

	_viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

    // ...
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-2-1.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

```cpp
// 기본화면

void Engine::Init(const WindowInfo& info)
{
	_window = info;
	ResizeWindow(info.width, info.height);

	_viewport = { 0, 0, static_cast<FLOAT>(info.width/2), static_cast<FLOAT>(info.height/2), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

    // ...
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-2-2.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

```cpp
// 기본화면

void Engine::Init(const WindowInfo& info)
{
	_window = info;
	ResizeWindow(info.width, info.height);

	_viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, info.width/2, info.height/2);

    // ...
```
<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-2-3.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

* 길게봤지만 사실상 viewport, scissor를 다르게 만들이유는 없다. 그냥 같은 width, heigh로 두고 쓴다 생각하면 된다.

---

## TIPS2) Descriptor Heap에 관하여 ...

* 사실 역할이 정확히 뭔지 잘 모르겠음

```cpp
void DescriptorHeap::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{
	_swapChain = swapChain;

    // device를 통해 Descriptor Heap을 RTV형태로 만든다.
	_rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDesc.NodeMask = 0;

	device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&_rtvHeap));
        // 여기까지가 Descriptor Heap을 만드는 과정

    // Descriptor Heap의 시작위치를 받아오고
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();

	for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
	{
        // 받아온 시작위치를 기반으로 View를 만든다
		_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * _rtvHeapSize);
		device->CreateRenderTargetView(swapChain->GetRenderTarget(i).Get(), nullptr, _rtvHandle[i]);
	}
}
```