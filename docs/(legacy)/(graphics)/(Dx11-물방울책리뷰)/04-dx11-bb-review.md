---
layout: default
title: "4. 성 만들어 보기"
parent: "(DirectX11 물방울책 리뷰)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/10)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/bb/bb-4-1.png"/>
</p>

* 여기도 Box와 크게 다를게 없음.
* 주목 할 만한 점이라면 아래와 같이 Index기반으로 그릴수 있다는 정도?

```cpp
void ShapesDemo::DrawScene()
{
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	_deviceContext->IASetInputLayout(_inputLayout.Get());
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_deviceContext->RSSetState(_wireframeRS.Get());

	uint32 stride = sizeof(Vertex);
	uint32 offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, _vertedBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set constants
	XMMATRIX view = ::XMLoadFloat4x4(&_view);
	XMMATRIX proj = ::XMLoadFloat4x4(&_proj);
	XMMATRIX viewProj =  view * proj;

	D3DX11_TECHNIQUE_DESC techDesc;
	_tech->GetDesc(&techDesc);

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the grid.
		XMMATRIX world = XMLoadFloat4x4(&_gridWorld);
		XMMATRIX wvp = world * viewProj;
		_fxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(wvp)));
		_tech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
		_deviceContext->DrawIndexed(_gridIndexCount, _gridIndexOffset, _gridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&_boxWorld);
		wvp = world * viewProj;
		_fxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(wvp)));
		_tech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
		_deviceContext->DrawIndexed(_boxIndexCount, _boxIndexOffset, _boxVertexOffset);

        // 이런식으로 부분 부분 나눠서 그릴 수 있다.

        // ...
```
