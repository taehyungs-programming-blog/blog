---
layout: default
title: "9. Map에도 Texture반영"
parent: "(DirectX11 물방울책 리뷰)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/13)

<사진9>

* 여긴 Wave Texture를 어떻게 움직이냐가 핵심

```cpp
void TexturedHillsAndWavesDemo::UpdateScene(float dt)
{
	// ...

	//
	// Animate water texture coordinates.
	//

	// Tile water texture.
	XMMATRIX wavesScale = ::XMMatrixScaling(5.0f, 5.0f, 0.0f);

	// Translate texture over time.
	_waterTexOffset.y += 0.05f * dt;
	_waterTexOffset.x += 0.1f * dt;
	XMMATRIX wavesOffset = ::XMMatrixTranslation(_waterTexOffset.x, _waterTexOffset.y, 0.0f);

	// Combine scale and translation.
	::XMStoreFloat4x4(&_waterTexTransform, wavesScale * wavesOffset);
}
```