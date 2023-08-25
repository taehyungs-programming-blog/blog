---
layout: default
title: "8. Texture 반영해보기"
parent: "(DirectX11 물방울책 리뷰)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/11)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/bb/bb-8-1.png"/>
</p>

```cpp
bool CrateDemo::Init()
{
	if (!App::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(_device, L"../Shaders/08. Basic.fx");
	InputLayouts::InitAll(_device);

    // Texture를 읽어오고
	DirectX::TexMetadata md;
	DirectX::ScratchImage img;
	HRESULT hr = ::LoadFromWICFile(L"../Resources/Textures/WoodCrate01.dds", WIC_FLAGS_NONE, &md, img);
	CHECK(hr);

	// 여러 버전이 있음. (~CreateShaderResourceViewEx)
	hr = ::CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _diffuseMapSRV.GetAddressOf());
	CHECK(hr);

	BuildGeometryBuffers();

	return true;
}
```

```cpp
void CrateDemo::DrawScene()
{
	// ...

	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		_deviceContext->IASetVertexBuffers(0, 1, _boxVB.GetAddressOf(), &stride, &offset);
		_deviceContext->IASetIndexBuffer(_boxIB.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Draw the box.
		XMMATRIX world = ::XMLoadFloat4x4(&_boxWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world * view * proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&_texTransform));
		Effects::BasicFX->SetMaterial(_boxMat);

        // 가져온 Texture를 DiffustMap에 반영
		Effects::BasicFX->SetDiffuseMap(_diffuseMapSRV.Get());

		activeTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());
		_deviceContext->DrawIndexed(_boxIndexCount, _boxIndexOffset, _boxVertexOffset);
	}

	HR(_swapChain->Present(0, 0));
}
```

```cpp
float4 PS(VertexOut pin, uniform int gLightCount, uniform bool gUseTexure) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalW = normalize(pin.NormalW);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;

	// Default to multiplicative identity.
	float4 texColor = float4(1, 1, 1, 1);
	if (gUseTexure)
	{
		// Sample texture.
		texColor = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
	}

	//
	// Lighting.
	//

	float4 litColor = texColor;
	if (gLightCount > 0)
	{
		// Start with a sum of zero. 
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Sum the light contribution from each light source.  
		[unroll]
		for (int i = 0; i < gLightCount; ++i)
		{
			float4 A, D, S;
			ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEye,
				A, D, S);

			ambient += A;
			diffuse += D;
			spec += S;
		}

		// Modulate with late add.
		litColor = texColor * (ambient + diffuse) + spec;
	}

	// Common to take alpha from diffuse material and texture.
	litColor.a = gMaterial.Diffuse.a * texColor.a;

	return litColor;
}
```