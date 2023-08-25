---
layout: default
title: "19. Dynamic CubeMap"
parent: "(DirectX11 물방울책 리뷰)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/15)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/bb/bb-19-1.png"/>
</p>

* 주변환경을 반사할때 사용된다.
* 가운데 구를 보면 된다.
* 잉? 이거 Stencil아닌가?
    * 좀 다르다.
    * 이미 그려진 화면에 Cube를 만들어 자신이 반사할 Texture를 고른다고 생각하면 된다
* 반사할 물체의 Cube를 만들고 각 방향으로 사진을 찍어 반사할 물체에 대입하면 되겠군?

```cpp
void DynamicCubeMapDemo::DrawScene()
{
	ID3D11RenderTargetView* renderTargets[1];

	// Generate the cube map.
	_deviceContext->RSSetViewports(1, &_cubeMapViewport);
	for (int i = 0; i < 6; ++i)
	{
		// Clear cube map face and depth buffer.
		_deviceContext->ClearRenderTargetView(_dynamicCubeMapRTV[i].Get(), reinterpret_cast<const float*>(&Colors::Silver));
		_deviceContext->ClearDepthStencilView(_dynamicCubeMapDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Bind cube map face as render target.
		renderTargets[0] = _dynamicCubeMapRTV[i].Get();

        // Render Target을 _dynamicCubeMapDSV로 바꿔두고
		_deviceContext->OMSetRenderTargets(1, renderTargets, _dynamicCubeMapDSV.Get());

		// Draw the scene with the exception of the center sphere to this cube map face.
        // 그려달라 명령
            // _cubeMapCamera에는 카메라의 방향이 들어가 있다
		DrawScene(_cubeMapCamera[i], false);
	}

    // Restore old viewport and render targets.
    // renderTargets에 반사될 Texture가 들어갔다
	_deviceContext->RSSetViewports(1, &_viewport);
	renderTargets[0] = _renderTargetView.Get();
	_deviceContext->OMSetRenderTargets(1, renderTargets, _depthStencilView.Get());

    // ...
```

