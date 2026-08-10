---
layout: default
title: "03-01. 삼각형을 그려보자 (사전 수정 사항)"
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

* 기존코드에서 약간의 수정이 필요하다

## (수정사항1) DescriptorHeap과 SwapChain을 통합

* 아직은 DescriptorHeap의 역할이 그려질 RenderTarget을 담고 있기만 하니 SwapChain에서 함께 관리해도 되겠다.

```cpp
class SwapChain
{
public:
	void Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	void Present();
	void SwapIndex();

	ComPtr<IDXGISwapChain> GetSwapChain() { return _swapChain; }
	ComPtr<ID3D12Resource> GetRenderTarget(int32 index) { return _rtvBuffer[index]; }

	ComPtr<ID3D12Resource> GetBackRTVBuffer() { return _rtvBuffer[_backBufferIndex]; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetBackRTV() { return _rtvHandle[_backBufferIndex]; }

private:
	void CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	void CreateRTV(ComPtr<ID3D12Device> device);

private:
	ComPtr<IDXGISwapChain>	_swapChain;
	
    // 이제 SwapChain에서 관리한다.
	ComPtr<ID3D12Resource>			_rtvBuffer[SWAP_CHAIN_BUFFER_COUNT];
	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle[SWAP_CHAIN_BUFFER_COUNT];

	uint32					_backBufferIndex = 0;
};

```

* 추가적으로 ... RenderTarget의 사용은

```cpp
void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackRTVBuffer().Get(),
		D3D12_RESOURCE_STATE_PRESENT, // 화면 출력
		D3D12_RESOURCE_STATE_RENDER_TARGET); // 외주 결과물

    // ...
```

---

## (수정사항2) 자주 사용될 device, commandqueue, swapchain을 매크로로 빼자

```cpp
class Engine
{
public:
    void Init(const WindowInfo& info);
    void Render();

public:
    shared_ptr<Device> GetDevice() { return _device; }
    shared_ptr<CommandQueue> GetCmdQueue() { return _cmdQueue; }
    shared_ptr<SwapChain> GetSwapChain() { return _swapChain; }
    shared_ptr<RootSignature> GetRootSignature() { return _rootSignature; }
```

```cpp
// EnginePch.h

// ...

#define DEVICE          GEngine->GetDevice()->GetDevice()
#define CMD_LIST        GEngine->GetCmdQueue()->GetCmdList()        // Command List도 자주사용되기에 별도로 뺌.
#define ROOT_SIGNATURE  GEngine->GetRootSignature()->GetSignature()

// ...
```
