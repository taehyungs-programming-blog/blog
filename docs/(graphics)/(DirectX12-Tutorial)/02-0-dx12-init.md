---
layout: default
title: "02-00. 장치 초기화"
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
* **(23.12.11)** 내용이 많아서 분리함.

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

* DirectX에서의 **Viewport와 Scissor Rect**는 화면에 그려지는 콘텐츠의 위치와 크기를 정의하는데 사용됩니다.
* **Viewport :**
	* Viewport는 화면에 그려질 3D 콘텐츠의 영역을 정의합니다.
	* 이것은 3D 월드에서의 콘텐츠를 2D 스크린 상에 투영하는 데 사용되는 창(window)의 역할을 합니다.
	* Viewport를 통해 3D 장면의 어떤 부분이 실제로 사용자에게 보여질지 결정할 수 있습니다.
	* Viewport의 크기와 위치를 변경함으로써, 화면에 표시되는 장면의 일부분을 확대하거나 축소할 수 있습니다.
* **Scissor Rect :**
	* Scissor Rect은 Viewport 내에서 더욱 세부적으로 픽셀을 그리거나 그리지 않을 영역을 정의합니다.
	* 이 기능을 사용하면 Viewport 내의 **특정 영역만을 렌더링**하는 것이 가능해집니다.
	* 예를 들어, 사용자 인터페이스 요소나 특정 게임 영역에만 렌더링을 제한하고자 할 때 유용합니다.
	* Scissor Rect은 Viewport와 함께 사용되어 렌더링의 효율성을 높이고, 필요한 부분에만 자원을 집중할 수 있도록 도와줍니다.
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

