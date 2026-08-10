---
layout: default
title: "11. Mirror 구현 (Stencil 써보기)"
parent: "(DirectX11 물방울책 리뷰)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/11)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/bb/bb-11-1.png"/>
</p>

* 핵심은 Stencil을 이용해 Mirror를 구현하는 것이다.
    * 추가적으론 그림자도 구현!

```cpp
// 우선 RenderState를 어떻게 수정했나 보자

D3D11_DEPTH_STENCIL_DESC mirrorDesc;
mirrorDesc.DepthEnable = true;
mirrorDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
mirrorDesc.DepthFunc = D3D11_COMPARISON_LESS;
mirrorDesc.StencilEnable = true;            // Stencil을 쓸께요
mirrorDesc.StencilReadMask = 0xff;
mirrorDesc.StencilWriteMask = 0xff;

mirrorDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
mirrorDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
mirrorDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
mirrorDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

// We are not rendering backfacing polygons, so these settings do not matter.
mirrorDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
mirrorDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
mirrorDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
mirrorDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

HR(device->CreateDepthStencilState(&mirrorDesc, &MarkMirrorDSS));
```

```cpp
//
// Draw the mirror to stencil buffer only.
//

activeTech->GetDesc(&techDesc);
for (uint32 p = 0; p < techDesc.Passes; ++p)
{
    ID3DX11EffectPass* pass = activeTech->GetPassByIndex(p);

    _deviceContext->IASetVertexBuffers(0, 1, _roomVB.GetAddressOf(), &stride, &offset);

    // Set per object constants.
    XMMATRIX world = ::XMLoadFloat4x4(&_roomWorld);
    XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
    XMMATRIX worldViewProj = world * view * proj;

    Effects::BasicFX->SetWorld(world);
    Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
    Effects::BasicFX->SetWorldViewProj(worldViewProj);
    Effects::BasicFX->SetTexTransform(XMMatrixIdentity());

    // Do not write to render target.
        // RenderTarget에 직접그리지 않겠다 선언
    _deviceContext->OMSetBlendState(RenderStates::NoRenderTargetWritesBS.Get(), blendFactor, 0xffffffff);

    // Render visible mirror pixels to stencil buffer.
    // Do not write mirror depth to depth buffer at this point, otherwise it will occlude the reflection.
        // Mirror를 Draw하며 위에서 선언한 DSV를 선택
    _deviceContext->OMSetDepthStencilState(RenderStates::MarkMirrorDSS.Get(), 1 /*Stencil을 1로 밀어달라*/);

    pass->Apply(0, _deviceContext.Get());
    _deviceContext->Draw(6, 24);

    // Restore states.
        // 다시 Rendering Pipeline의 Stencil을 기존값으로 복원
    _deviceContext->OMSetDepthStencilState(0, 0);
    _deviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);
}
```

* 여기까지 하면 Mirror에 Stencil값을 1로 밀어둔 상태이다.
* 이제 반사되는 해골을 그려보자.

```cpp
//
// Draw the skull reflection.
//

//...

	// Only draw reflection into visible mirror pixels as marked by the stencil buffer. 
        // Stencil이 1과 같은 부분만 그려달라는 명령
	_deviceContext->OMSetDepthStencilState(RenderStates::DrawReflectionDSS.Get(), 1);
	pass->Apply(0, _deviceContext.Get());
	_deviceContext->DrawIndexed(_skullIndexCount, 0, 0);

	// Restore default states.
	_deviceContext->RSSetState(0);
	_deviceContext->OMSetDepthStencilState(0, 0);

	// Restore light directions.
	for (int i = 0; i < 3; ++i)
	{
		_dirLights[i].Direction = oldLightDirections[i];
	}

	Effects::BasicFX->SetDirLights(_dirLights);
}
```

* 거울에 알파블렌드 적용

```cpp
//
// Draw the mirror to the back buffer as usual but with transparency
// blending so the reflection shows through.
// 

activeTech->GetDesc(&techDesc);
for (uint32 p = 0; p < techDesc.Passes; ++p)
{
    ID3DX11EffectPass* pass = activeTech->GetPassByIndex(p);

    _deviceContext->IASetVertexBuffers(0, 1, _roomVB.GetAddressOf(), &stride, &offset);

    // Set per object constants.
    XMMATRIX world = ::XMLoadFloat4x4(&_roomWorld);
    XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
    XMMATRIX worldViewProj = world * view * proj;

    Effects::BasicFX->SetWorld(world);
    Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
    Effects::BasicFX->SetWorldViewProj(worldViewProj);
    Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
    Effects::BasicFX->SetMaterial(_mirrorMat);
    Effects::BasicFX->SetDiffuseMap(_mirrorDiffuseMapSRV.Get());

    // Mirror
    _deviceContext->OMSetBlendState(RenderStates::TransparentBS.Get(), blendFactor, 0xffffffff);
    pass->Apply(0, _deviceContext.Get());
    _deviceContext->Draw(6, 24);
}

```

* 마지막으로 그림자 그리기

```cpp
//
// Draw the skull shadow.
//
activeSkullTech->GetDesc(&techDesc);
for (uint32 p = 0; p < techDesc.Passes; ++p)
{
    ID3DX11EffectPass* pass = activeSkullTech->GetPassByIndex(p);

    _deviceContext->IASetVertexBuffers(0, 1, _skullVB.GetAddressOf(), &stride, &offset);
    _deviceContext->IASetIndexBuffer(_skullIB.Get(), DXGI_FORMAT_R32_UINT, 0);

    XMVECTOR shadowPlane = ::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // xz plane
    XMVECTOR toMainLight = -::XMLoadFloat3(&_dirLights[0].Direction);
    XMMATRIX S = ::XMMatrixShadow(shadowPlane, toMainLight);
    XMMATRIX shadowOffsetY = ::XMMatrixTranslation(0.0f, 0.001f, 0.0f);

    // Set per object constants.
    XMMATRIX world = ::XMLoadFloat4x4(&_skullWorld) * S * shadowOffsetY;
    XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
    XMMATRIX worldViewProj = world * view * proj;

    Effects::BasicFX->SetWorld(world);
    Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
    Effects::BasicFX->SetWorldViewProj(worldViewProj);
    Effects::BasicFX->SetMaterial(_shadowMat);

    // 두번은 그리지 마라 선언해야 깔끔하게 나옴
    _deviceContext->OMSetDepthStencilState(RenderStates::NoDoubleBlendDSS.Get(), 0);
    pass->Apply(0, _deviceContext.Get());
    _deviceContext->DrawIndexed(_skullIndexCount, 0, 0);

    // Restore default states.
    _deviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);
    _deviceContext->OMSetDepthStencilState(0, 0);
}
```