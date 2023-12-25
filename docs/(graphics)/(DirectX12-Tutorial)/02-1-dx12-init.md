---
layout: default
title: "02-01. 장치 초기화 (Device)"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

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
    * 대표적으로 SwapChain을 생성할때 사용된다.
* Graphic Driver Enum
    * 말 그대로 Grphic Driver 정보를 열거한다.
* Present
    * Swapchain 만들기
    * FullScreen 전환
    * 창크기 전환 등

* 사실 Device가 뭘하는지 잘모르겠는데 그냥 dxgi, device를 생성하고 관리한다 정도만 알아도 된다.

---

## dxgi에 대해서 ...

* **DXGI (DirectX Graphics Infrastructure)** - 이 API는 주로 그래픽 하드웨어와 상호 작용하기 위한 인터페이스를 제공합니다. 
* DXGI는 DirectX 10 이상에서 사용되며, 그래픽 장치 관리, 출력 디스플레이 관리 및 고성능 렌더링을 위한 효율적인 방법을 제공합니다.

* DXGI의 주요 기능
    * **장치 관리(Device Management)**: DXGI는 그래픽 카드와 같은 하드웨어 장치를 관리합니다. 이를 통해 개발자는 다양한 그래픽 장치에서 애플리케이션을 실행할 수 있습니다.
    * **스왑 체인(Swap Chains)**: 스왑 체인은 렌더링된 이미지를 화면에 표시하기 위해 사용됩니다. 이것은 더블 버퍼링(double buffering) 또는 트리플 버퍼링(triple buffering) 같은 기술을 사용하여 화면 깜빡임이나 찢어짐 없이 매끄러운 이미지 전환을 가능하게 합니다.
    * **출력(Output)**: DXGI는 다양한 디스플레이 장치를 지원하며, 다중 모니터 설정을 관리하는 기능도 제공합니다.
    * **풀스크린과 윈도우 모드(Fullscreen and Windowed Modes)**: DXGI는 풀스크린과 윈도우 모드 사이의 전환을 처리합니다. 이는 게임과 같은 고성능 그래픽 애플리케이션에서 중요한 기능입니다.
    * **리소스 공유(Resource Sharing)**: DXGI를 사용하면 다른 DirectX 컴포넌트와 리소스를 공유할 수 있습니다. 이를 통해 효율적인 메모리 관리와 더 나은 성능을 달성할 수 있습니다.

### 장치 관리(Device Management)

```cpp
#include <d3d12.h>

// ...

ID3D12Device* device;
D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

// 사용 후 device를 해제합니다.
device->Release();

```

---

### 스왑 체인(Swap Chains)

```cpp
#include <dxgi1_6.h>
#include <d3d12.h>

// ...

IDXGISwapChain1* swapChain;

DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
swapChainDesc.BufferCount = 2;
swapChainDesc.Width = 800;
swapChainDesc.Height = 600;
swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
swapChainDesc.SampleDesc.Count = 1;

IDXGIFactory4* factory;
CreateDXGIFactory1(IID_PPV_ARGS(&factory));
factory->CreateSwapChainForHwnd(
    // ...
    hwnd, // 윈도우 핸들
    &swapChainDesc,
    nullptr,
    nullptr,
    &swapChain
);

// 사용 후 자원 해제
swapChain->Release();
factory->Release();

```

---

### 출력(Output)

```cpp
#include <dxgi1_6.h>

// ...

IDXGIFactory1* factory;
CreateDXGIFactory1(IID_PPV_ARGS(&factory));

IDXGIAdapter1* adapter;
IDXGIOutput* output;
for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
    for (UINT j = 0; adapter->EnumOutputs(j, &output) != DXGI_ERROR_NOT_FOUND; ++j) {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);
        // desc를 사용하여 출력 장치 정보 출력

        output->Release();
    }
    adapter->Release();
}

factory->Release();

```

---

### 풀스크린과 윈도우 모드(Fullscreen and Windowed Modes)

```cpp
swapChain->SetFullscreenState(TRUE, nullptr); // 풀스크린 모드로 전환
// ...

swapChain->SetFullscreenState(FALSE, nullptr); // 윈도우 모드로 전환

```

---

### 리소스 공유(Resource Sharing)

```cpp
ID3D12Resource* sharedTexture;

// 첫 번째 디바이스에서 리소스 생성
device1->CreateCommittedResource(
    // ...
    &sharedTexture
);

// 두 번째 디바이스에서 공유 핸들을 사용하여 리소스를 열기
HANDLE sharedHandle;
device1->CreateSharedHandle(sharedTexture, nullptr, GENERIC_ALL, L"SharedTexture", &sharedHandle);
device2->OpenSharedHandle(sharedHandle, IID_PPV_ARGS(&sharedTexture));

// ...

// 사용 후 리소스 해제
sharedTexture->Release();

```

