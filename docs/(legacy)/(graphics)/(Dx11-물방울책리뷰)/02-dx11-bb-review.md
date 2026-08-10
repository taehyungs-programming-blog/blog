---
layout: default
title: "2. Box 띄우기"
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

## Box 띄워 보기

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/bb/bb-2-1.png"/>
</p>

* 사실상 Init과 DrawScene만 보면 되는데

```cpp
bool BoxDemo::Init()
{
	if (!App::Init())
		return false;

    // Geo Buffer 생성
	BuildGeometryBuffers();
    // Shader Compile
	BuildFX();
    // Shader로 넘길 변수 초기화
	BuildVertexLayout();

	return true;
}
```

```cpp
void BoxDemo::BuildFX()
{
	// ...

	_tech = _fx->GetTechniqueByName("T0");

    // Dx11 Effect로 gWorldViewProj 즉 View Matrix를 보낸다
	_fxWorldViewProj = _fx->GetVariableByName("gWorldViewProj")->AsMatrix();
}
```

```cpp
void BoxDemo::DrawScene()
{
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	_deviceContext->IASetInputLayout(_inputLayout.Get());
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	uint32 stride = sizeof(Vertex);
	uint32 offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set constants
	XMMATRIX world = ::XMLoadFloat4x4(&_world);
	XMMATRIX view = ::XMLoadFloat4x4(&_view);
	XMMATRIX proj = ::XMLoadFloat4x4(&_proj);
	XMMATRIX worldViewProj = world * view * proj;

	_fxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	D3DX11_TECHNIQUE_DESC techDesc;
	_tech->GetDesc(&techDesc);
	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		_tech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());

		// 36 indices for the box.
		_deviceContext->DrawIndexed(36, 0, 0);
	}

	HR(_swapChain->Present(0, 0));
}
```

---

## Q. 마우스로 이동된 움직임을 어떻게 반영할까?

```cpp
void BoxDemo::UpdateScene(float dt)
{
    // 카메라의 움직임을 보정해
	// Convert Spherical to Cartesian coordinates.
	float x = _radius * sinf(_phi) * cosf(_theta);
	float z = _radius * sinf(_phi) * sinf(_theta);
	float y = _radius * cosf(_phi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    // View에 반영
	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&_view, V);
}
```