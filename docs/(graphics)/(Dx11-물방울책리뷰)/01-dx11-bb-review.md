---
layout: default
title: "1. 도형 띄워보기"
parent: "(DirectX11 물방울책 리뷰)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 모두 정리하려했지만 ... 중복된 부분이 많아서 중요 하다 혹은 몰랐던 부분만 정리합니다.

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/12)

## Rendering Pipeline

<사진>
https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-graphics-pipeline

* **Input Assembler, IA**
    * 정점 데이터를 받아들입니다.
    * 입력된 데이터는 포인트 리스트, 라인 리스트, 트라이앵글 리스트 등의 형태로 조립됩니다.
* **Vertex Shader, VS**
    * 각 정점에 대해 수행되는 프로그램입니다.
    * 3D 좌표 변환, 조명 계산 등의 작업을 수행할 수 있습니다.
* **Tessellation**
    * (선택적 단계) 모델의 세부 정점을 동적으로 생성하여 상세도를 높입니다.
    * 테셀레이션 제어 셰이더 (Tessellation Control Shader, TCS)와 테셀레이션 평가 셰이더 (Tessellation Evaluation Shader, TES)로 구성됩니다.
* **Geometry Shader, GS**
    * (선택적 단계) 정점 데이타를 이용하여 새로운 프리미티브를 생성하거나 제거할 수 있습니다.
* **Stream Output, SO**
    * 셰이더에서 출력된 데이터를 메모리 버퍼에 직접 캡쳐합니다.
    * 후속 렌더링 호출에서 재사용 가능합니다.
* **Rasterizer**
    * 정점 데이터를 화면의 픽셀로 변환합니다.
    * 뒷면 제거, 클리핑, 시야각 큐링 등의 작업을 수행합니다.
* **Pixel Shader, PS**
    * 화면의 각 픽셀에 대해 수행되는 프로그램입니다.
    * 텍스처 매핑, 조명, 셰이딩 등의 작업을 수행하여 최종 색상 값을 결정합니다.
* **Output Merger, OM**
    * 픽셀 셰이더의 출력과 현재의 프레임 버퍼를 병합합니다.
    * 깊이, 스텐실 테스트, 블렌딩 연산 등이 이루어집니다.

---

## 도형 띄워 보기

* 간단한 부분은 생략

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

```cpp
void WavesDemo::BuildWavesGeometryBuffers()
{
	D3D11_BUFFER_DESC vbd;
    // CPU에서 Vertex수정이 필요한 경우 이렇게 설정 1
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * _waves.VertexCount();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    // CPU에서 Vertex수정이 필요한 경우 이렇게 설정 2
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	HR(_device->CreateBuffer(&vbd, 0, _wavesVB.GetAddressOf()));

    // ...
```