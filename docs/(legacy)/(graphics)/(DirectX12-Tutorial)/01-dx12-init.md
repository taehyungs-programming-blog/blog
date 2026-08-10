---
layout: default
title: "01. 프로젝트 설정"
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

## 랜더링 파이프라인 살펴보기

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-1-0.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

* 들어가기 전 알아 둘 점
    * 3D 그래픽은 오브젝트를 그릴때 오브젝트를 **삼각형(정점)**으로 관리한다.
    * 대략 한 프레임이 그려질때 위 **파이프라인**이 돌아간다고 생각하면된다.
* **Input-Assembler Stage** : 정점정보(가 어떻게 구성되어 있는지) 전달
* **Vertex Shader Stage** : 정점정보를 연산(좌표이동 등), 카메라의 위치에 따라 정점이 어떻게 보일지 등
* **Tessellation Stage(DX11~)** : 새로운 정점을 계산(경우에 따라 정점이 추가/제거되어야 할 필요가 있음(Ex> 멀리있는 산의 표현))
    * **Hull Shader**
    * **Tesselator Stage**
    * **Domain Stage**
* **Geometry Shader Stage(DX11~)** : 새로운 정점을 계산, Ex) 가까이 있는 물체의 경우 새로운 정점이 필요
    * << 여기까진 정점계산, 아래서부턴 Pixel 색상 계산 >>
* **Rasterize Stage** : 여기부터 색상을 입힌다, 정점의 색상 정보 관리(A, B점으로 가며 색상을 어떻게 변경하는지 정점간 선을 어떻게 그을지 등)
* **Pixel Shader Stage** : 색상을 넣는다
* **Output-Merger Stage** : 모든 색상을 조합

---

## 게임컨텐츠용 프로젝트 생성

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/2)

* 환경
    * VisualStudio 2019

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-1-1.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

* 메시지루프를 약간 수정하자

```cpp
// 기본 메시지 루프입니다:
while (true)
{
    // 이후 Update를 통해 주기적으로 돌아야 하기에
    // 메시지가 들어올때만 받는게 아니라 있는지를 체크후 꺼내는 형태로 변경
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            break;

        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // TODO : 게임로직
}
```

* 미리 컴파일된 헤더 사용하게 변경

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-1-2.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

```cpp
// pch.h
#pragma once

#include <vector>
#include <memory>
using namespace std;
```

```cpp
// directx-example.cpp 
#include "pch.h"
// ...
```

---

### 호출될 Game class(게임컨텐츠 클래스) 생성

```cpp
// game.h

#pragma once
class Game
{
public:
	void Init();
	void Update();
};
```

```cpp
// game.cpp

#include "pch.h"
#include "Game.h"

void Game::Init()
{
}

void Game::Update()
{
}
```

* Game을 주기적으로 Update

```cpp
// main.cpp

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)

// ...

MSG msg;

//Game* game = new Game();
unique_ptr<Game> game = make_unique<Game>();
game->Init();

// 기본 메시지 루프입니다:
while (true)
{
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            break;

        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // TODO : 게임로직
    game->Update();
}

// ...
```

---

### DirectX 라이브러리용 프로젝트 생성

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-1-3.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

* 역시 미리컴파일된 헤더 추가

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-1-2.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

* d3dx12를 쓸것이기에 [d3dx12.h](https://github.com/microsoft/DirectX-Headers/blob/main/include/directx/d3dx12.h)를 다운받아서 프로젝트에 넣자

* `pch.h`에 아래를 추가

```cpp
// pch.h

#pragma once

// 각종 include
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
using namespace std;

#include "d3dx12.h"
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

// 각종 lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

// 각종 typedef
using int8		= __int8;
using int16		= __int16;
using int32		= __int32;
using int64		= __int64;
using uint8		= unsigned __int8;
using uint16	= unsigned __int16;
using uint32	= unsigned __int32;
using uint64	= unsigned __int64;
using Vec2		= XMFLOAT2;
using Vec3		= XMFLOAT3;
using Vec4		= XMFLOAT4;
using Matrix	= XMMATRIX;

void HelloEngine();
```

* 마지막으로 출력디렉터리만 수정해보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-1-4.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

* 끝? 링킹설정은??

```cpp
// pch.cpp
#pragma once

// 프로젝트마다 매번 설정하기 귀찮기에 이렇게 하자
#pragma comment(lib, "Engine.lib")

#include "EnginePch.h"
```

---

## 정리하자면 ...

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-1-5.png"/>
</p>

* Client - 게임컨텐츠를 담당하는 프로젝트
* Engine - DirectX 라이브러리를 담당하는 프로젝트

```cpp
// Client.cpp

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    // ...

    unique_ptr<Game> game = make_unique<Game>();
    game->Init(GWindowInfo);

    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }

		// TODO
        game->Update();
    }
```

```cpp
class Engine
{
public:

	void Init(const WindowInfo& info);
	void Render();

public:
	void RenderBegin();
	void RenderEnd();

	void ResizeWindow(int32 width, int32 height);

private:
	// 그려질 화면 크기 관련
	WindowInfo		_window;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};

	shared_ptr<class Device>            _device;
	shared_ptr<class CommandQueue>      _cmdQueue;
	shared_ptr<class SwapChain>         _swapChain;
	shared_ptr<class DescriptorHeap>    _descHeap;
};
```