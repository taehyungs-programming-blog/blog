---
layout: default
title: "16. Instancing & Culling"
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

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/12)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/bb/bb-16-1.png"/>
</p>

## Instancing

* Instancing - 같은 물체를 모아서 그려보자

```cpp
void InstancingAndCullingDemo::DrawScene()
{
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Silver));
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	_deviceContext->IASetInputLayout(InputLayouts::InstancedBasic32.Get());
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	uint32 stride[2] = { sizeof(Vertex::Basic32), sizeof(InstancedData) };
	uint32 offset[2] = { 0,0 };

	ID3D11Buffer* vbs[2] = { _skullVB.Get(), _instancedBuffer.Get() };

	XMMATRIX view = _camera.View();
	XMMATRIX proj = _camera.Proj();
	XMMATRIX viewProj = _camera.ViewProj();

	// Set per frame constants.
	Effects::InstancedBasicFX->SetDirLights(_dirLights);
	Effects::InstancedBasicFX->SetEyePosW(_camera.GetPosition());

	ComPtr<ID3DX11EffectTechnique> activeTech = Effects::InstancedBasicFX->Light3Tech;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (uint32 p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the skull.

        // Input Assembler 단계에서 Buffer가 2개 넘어옴을 알린다
		_deviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
		_deviceContext->IASetIndexBuffer(_skullIB.Get(), DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world = XMLoadFloat4x4(&_skullWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);

		Effects::InstancedBasicFX->SetWorld(world);
		Effects::InstancedBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::InstancedBasicFX->SetViewProj(viewProj);
		Effects::InstancedBasicFX->SetMaterial(_skullMat);

		activeTech->GetPassByIndex(p)->Apply(0, _deviceContext.Get());

        // DrawIndexedInstanced로 그려달라 요청함을 주목
		_deviceContext->DrawIndexedInstanced(_skullIndexCount, _visibleObjectCount/*개 그려주세요*/, 0, 0, 0);
	}

	HR(_swapChain->Present(0, 0));
}
```

---

## Culling

* CPU단에서 처리한다

```cpp
void InstancingAndCullingDemo::UpdateScene(float dt)
{
	// ...

	//
	// Perform frustum culling.
	//

	_camera.UpdateViewMatrix();
	_visibleObjectCount = 0;

	if (_frustumCullingEnabled)
	{
		XMVECTOR detView = XMMatrixDeterminant(_camera.View());
		XMMATRIX invView = XMMatrixInverse(&detView, _camera.View());

		D3D11_MAPPED_SUBRESOURCE mappedData;
		_deviceContext->Map(_instancedBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		InstancedData* dataView = reinterpret_cast<InstancedData*>(mappedData.pData);

		for (uint32 i = 0; i < _instancedData.size(); ++i)
		{
			XMMATRIX W = ::XMLoadFloat4x4(&_instancedData[i].World);
			XMVECTOR D = ::XMMatrixDeterminant(W);
			XMMATRIX invWorld = ::XMMatrixInverse(&D, W);

			// View space to the object's local space.
			XMMATRIX toLocal = ::XMMatrixMultiply(invView, invWorld);

			// Decompose the matrix into its individual parts.
			XMVECTOR scale;
			XMVECTOR rotQuat;
			XMVECTOR translation;
			::XMMatrixDecompose(&scale, &rotQuat, &translation, toLocal);

			// Transform the camera frustum from view space to the object's local space.
			BoundingFrustum localspaceFrustum;

			_camFrustum.Transform(localspaceFrustum, XMVectorGetX(scale), rotQuat, translation);
			//XNA::TransformFrustum(&localspaceFrustum, &_camFrustum, );

			// Perform the box/frustum intersection test in local space.
			if (localspaceFrustum.Contains(_skullBox))
			{
				// Write the instance data to dynamic VB of the visible objects.
                    // 그릴애들만 넣는다
				dataView[_visibleObjectCount++] = _instancedData[i];
			}
		}

		_deviceContext->Unmap(_instancedBuffer.Get(), 0);
	}
	else // No culling enabled, draw all objects.
	{
		D3D11_MAPPED_SUBRESOURCE mappedData;
		_deviceContext->Map(_instancedBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		InstancedData* dataView = reinterpret_cast<InstancedData*>(mappedData.pData);

		for (uint32 i = 0; i < _instancedData.size(); ++i)
		{
			dataView[_visibleObjectCount++] = _instancedData[i];
		}

		_deviceContext->Unmap(_instancedBuffer.Get(), 0);
	}

	std::wostringstream outs;
	outs.precision(6);
	outs << L"Instancing and Culling Demo" <<
		L"    " << _visibleObjectCount <<
		L" objects visible out of " << _instancedData.size();

	_mainWindowCaption = outs.str();
}
```