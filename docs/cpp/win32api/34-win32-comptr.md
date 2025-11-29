---
layout: default
title: "34. ComPtr, QueryInterface"
parent: (Windows API)
grand_parent: (C++)
nav_order: 4
---

## ComPtr이란

* **ComPtr(COM Smart Pointer)**은 Microsoft의 Windows Runtime C++ Template Library (WRL)에서 제공하는 스마트 포인터
    * COM(Component Object Model) 객체의 수명을 자동으로 관리해주는 템플릿 클래스

### 주요 특징

* 자동 참조 카운팅 관리
    * COM 객체는 참조 카운팅 방식으로 수명을 관리합니다
    * ComPtr은 AddRef()와 Release()를 자동으로 호출하여 메모리 누수를 방지합니다
    * 개발자가 수동으로 참조 카운트를 관리할 필요가 없습니다
* RAII 패턴
    * C++의 RAII(Resource Acquisition Is Initialization) 원칙을 따릅니다
    * 스코프를 벗어나면 자동으로 Release()가 호출됩니다
    * std::unique_ptr, std::shared_ptr과 유사한 사용 패턴

```cpp
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
```

```cpp
ID3D12Device* device = nullptr;
D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

// 사용...

if (device) {
    device->Release(); // 수동으로 Release 호출 필요!
}
```

```cpp
ComPtr<ID3D12Device> device;
D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

// 사용...
// 자동으로 Release 호출됨 (스코프 종료 시)
```

> 사실 쓰는 이유는 Win API를 손쉽게 쓰기 위함이다

```cpp
auto deleter = [](ID3D12Device* p) { if(p) p->Release(); };
std::unique_ptr<ID3D12Device, decltype(deleter)> device(nullptr, deleter);

// 1. Device 생성 - 복잡함!
ID3D12Device* rawDevice = nullptr;  // 임시 변수 필요
D3D12CreateDevice(
    nullptr,
    D3D_FEATURE_LEVEL_11_0,
    __uuidof(ID3D12Device),     // IID를 수동으로 지정
    (void**)&rawDevice          // void**로 캐스팅 필요
);
device.reset(rawDevice);  // 수동으로 unique_ptr에 넣어야 함

// 2. Command Queue 생성 - 더 복잡함!
auto queueDeleter = [](ID3D12CommandQueue* p) { if(p) p->Release(); };
std::unique_ptr<ID3D12CommandQueue, decltype(queueDeleter)> 
    commandQueue(nullptr, queueDeleter);  // 타입 선언도 길어짐
    
ID3D12CommandQueue* rawQueue = nullptr;  // 또 임시 변수
D3D12_COMMAND_QUEUE_DESC queueDesc = {};
device->CreateCommandQueue(
    &queueDesc,
    __uuidof(ID3D12CommandQueue),
    (void**)&rawQueue
);
commandQueue.reset(rawQueue);  // 또 수동 설정

// 3. QueryInterface - 매우 복잡함!
auto device5Deleter = [](ID3D12Device5* p) { if(p) p->Release(); };
std::unique_ptr<ID3D12Device5, decltype(device5Deleter)> 
    device5(nullptr, device5Deleter);
    
ID3D12Device5* rawDevice5 = nullptr;
device->QueryInterface(__uuidof(ID3D12Device5), (void**)&rawDevice5);
device5.reset(rawDevice5);
```

---

## QueryInterface란?

* QueryInterface는 COM(Component Object Model)의 핵심 메커니즘으로, 하나의 객체가 여러 인터페이스를 지원하는지 런타임에 확인하고, 해당 인터페이스 포인터를 얻는 기능

### 왜 필요한가?

* C++의 dynamic_cast와 비슷하지만, 이진 호환성(binary compatibility)을 유지하면서 인터페이스를 조회할 수 있습니다.

```
// 하나의 COM 객체는 여러 인터페이스를 구현할 수 있음
//
//     ┌─────────────────────────┐
//     │  ID3D12Device 객체      │
//     ├─────────────────────────┤
//     │ ID3D12Device            │ <- 기본 인터페이스
//     │ ID3D12Device1           │ <- Windows 10 추가 기능
//     │ ID3D12Device2           │ <- 더 많은 기능
//     │ ID3D12Device5           │ <- 최신 기능 (레이트레이싱 등)
//     │ IUnknown                │ <- 모든 COM 인터페이스의 기본
//     └─────────────────────────┘
```

```cpp
// IUnknown의 정의
interface IUnknown {
    // 1. QueryInterface - 다른 인터페이스 조회
    virtual HRESULT QueryInterface(
        REFIID riid,        // 원하는 인터페이스 ID
        void** ppvObject    // 결과를 받을 포인터
    ) = 0;
    
    // 2. AddRef - 참조 카운트 증가
    virtual ULONG AddRef() = 0;
    
    // 3. Release - 참조 카운트 감소
    virtual ULONG Release() = 0;
};
```

```cpp
// Device 생성 (ID3D12Device)
ID3D12Device* device = nullptr;
D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, 
                  IID_PPV_ARGS(&device));

// ID3D12Device5 인터페이스가 지원되는지 확인하고 얻기
ID3D12Device5* device5 = nullptr;
HRESULT hr = device->QueryInterface(
    __uuidof(ID3D12Device5),  // 원하는 인터페이스 ID
    (void**)&device5           // 결과 받을 포인터
);

if (SUCCEEDED(hr)) {
    // device5를 사용할 수 있음 (레이트레이싱 기능 등)
    device5->CreateStateObject(...);  // ID3D12Device5의 메서드
    
    device5->Release();  // 사용 후 반드시 Release
} else {
    // 이 시스템은 ID3D12Device5를 지원하지 않음
    // (Windows 버전이 낮거나, DirectX 업데이트가 안 됨)
}

device->Release();
```

* ComPtr 방식 (간단함)

```cpp
ComPtr<ID3D12Device> device;
D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

// As() 메서드로 간단하게!
ComPtr<ID3D12Device5> device5;
HRESULT hr = device.As(&device5);

if (SUCCEEDED(hr)) {
    // device5를 사용할 수 있음
    device5->CreateStateObject(...);
    
    // Release는 자동으로 호출됨
}

// device도 자동으로 Release됨
```

---

## COM(Component Object Model) 객체란?

* COM은 Microsoft가 1990년대에 만든 소프트웨어 컴포넌트 아키텍처
* 쉽게 말하면, 다른 언어, 다른 프로세스, 심지어 다른 컴퓨터에서도 객체를 사용할 수 있게 해주는 표준 규격

### 왜 COM이 필요했나?

> 문제 상황 (1990년대)

```cpp
// Windows API는 C 스타일 함수들
HWND CreateWindowEx(...);
BOOL SetWindowText(HWND hwnd, LPCSTR lpString);

// 문제점:
// 1. 객체 지향적이지 않음
// 2. 확장이 어려움
// 3. 다른 언어(VB, Delphi)에서 사용하기 어려움
// 4. DLL 버전 호환성 문제 (DLL Hell)
```

> 해결책

```cpp
// COM은 인터페이스 기반
interface IUnknown {
    virtual HRESULT QueryInterface(REFIID riid, void** ppv) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
};

// 장점:
// 1. 객체 지향적
// 2. 인터페이스로 확장 가능
// 3. 언어 독립적 (C++, C#, VB, Python, JavaScript 등)
// 4. 이진 호환성 보장 (버전 문제 해결)
```

```cpp
// ❌ 일반 C++ - 클래스에 직접 접근
class Device {
public:
    void CreateBuffer();
};

Device* device = new Device();  // 직접 생성
device->CreateBuffer();
delete device;

// ✅ COM - 인터페이스를 통해서만 접근
interface ID3D12Device : public IUnknown {
    virtual void CreateBuffer() = 0;
};

// 클래스 정의는 숨겨짐 (구현 세부사항)
class CDevice : public ID3D12Device {
    // 내부 구현
};

// 팩토리 함수로만 생성 가능
ID3D12Device* device = nullptr;
D3D12CreateDevice(..., &device);  // 인터페이스 포인터만 받음
device->CreateBuffer();
device->Release();  // delete 사용 안 함!
```

> 언어의 독립성 확보가능능

```cpp
// C++
ComPtr<ID3D12Device> device;
D3D12CreateDevice(..., &device);
device->CreateCommandQueue(...);

// C# (.NET)
var device = D3D12.CreateDevice(...);
device.CreateCommandQueue(...);

// Python
device = d3d12.CreateDevice(...)
device.CreateCommandQueue(...)

// JavaScript (Edge/IE에서)
var device = D3D12CreateDevice(...);
device.CreateCommandQueue(...);
```

> GUID

```cpp
// 각 인터페이스마다 고유한 GUID
// ID3D12Device의 GUID
// {189819f1-1db6-4b57-be54-1821339b85f7}

// QueryInterface에서 사용
GUID iid = __uuidof(ID3D12Device);
device->QueryInterface(iid, (void**)&devicePtr);

// 같은 의미
device->QueryInterface(
    IID_ID3D12Device,  // 미리 정의된 GUID
    (void**)&devicePtr
);
```