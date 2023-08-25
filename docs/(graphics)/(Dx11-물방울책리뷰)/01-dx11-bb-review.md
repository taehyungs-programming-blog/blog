---
layout: default
title: "1. Rendering Pipeline의 복습 + Init Dx11"
parent: "(DirectX11 물방울책 리뷰)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/10)

## Rendering Pipeline

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/bb/bb-1-1.png"/>
</p>

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

## Init Dx11

* **App** 클래스를 상속받아 Dx11이 초기화 되게 만들어 둠.
	* 코드를 보는게 오히려 편함

```cpp
// 유의할 점은 ClearRenderTarget과 Clear DSV는 컨텐츠 구현단에서 해줘야 한다.
void InitDemo::DrawScene()
{
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Blue));
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	HR(_swapChain->Present(0, 0));
}

```