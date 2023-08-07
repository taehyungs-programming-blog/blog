---
layout: default
title: "03. Framework로 만들기"
parent: "(DirectX11 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 이번 장에서 하고자 하는 것은 Game클래스에 몰빵되었던 코드를 나누는 것이다.
* 기능이 추가되는 것이 아니라 필요에 따라 [코드 🌍]()를 참고하자

## Graphics

```cpp
void Game::Render()
{
	_graphics->RenderBegin();

    // ...
```

```cpp
#pragma once

class Graphics
{
public:
	Graphics(HWND hwnd);
	~Graphics();

	void RenderBegin();
	void RenderEnd();

	ComPtr<ID3D11Device> GetDevice() { return _device; }
	ComPtr<ID3D11DeviceContext> GetDeviceContext() { return _deviceContext; }

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void SetViewport();

private:
	HWND _hwnd = {};

	// Device & SwapChain
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
	ComPtr<IDXGISwapChain> _swapChain = nullptr;

	// RTV
	ComPtr<ID3D11RenderTargetView> _renderTargetView;

	// Misc
	D3D11_VIEWPORT _viewport = { 0 };
	float _clearColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
};
```

---

## InputAssembler

* 세 가지가 있다.
	* IndexBuffer
	* InputLayout
	* VertexBuffer
* Index, Vertex는 쉬우니 InputLayout만 설명하자면
* Shader에 데이터가 어떻게 넘어가는지 알린다.

```cpp
vector<D3D11_INPUT_ELEMENT_DESC> VertexTextureData::descs = 
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
```

```cpp
void InputLayout::Create(const vector<D3D11_INPUT_ELEMENT_DESC>& descs, ComPtr<ID3DBlob> blob)
{
	const int32 count = static_cast<int32>(descs.size());
	_device->CreateInputLayout(descs.data(), count, blob->GetBufferPointer(), blob->GetBufferSize(), _inputLayout.GetAddressOf());
}
```
