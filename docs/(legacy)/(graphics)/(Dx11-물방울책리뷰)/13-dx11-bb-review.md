---
layout: default
title: "13. Compute Shader"
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

```cpp
struct Data
{
	float3 v1;
	float2 v2;
};

// StructuredBuffer는 배열이라 생각하자
StructuredBuffer<Data> gInputA;
StructuredBuffer<Data> gInputB;
RWStructuredBuffer<Data> gOutput;

[numthreads(32, 1, 1)]
void CS(int3 dtid : SV_DispatchThreadID)
{
    // input의 두개를 그냥 더해달라
	gOutput[dtid.x].v1 = gInputA[dtid.x].v1 + gInputB[dtid.x].v1;
	gOutput[dtid.x].v2 = gInputA[dtid.x].v2 + gInputB[dtid.x].v2;
}

technique11 VecAdd
{
	pass P0
	{
        // Vertex와 Pixel Shader는 사용하지 않는다
		SetVertexShader(NULL);
		SetPixelShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, CS()));
	}
}
```

---

## Gaussian Blur를 Compute Shader로 구현

* 가로, 세로 두 번 픽셀단위로 Gaussian Blur 반영

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/bb/bb-13-1.png"/>
</p>

```cpp
void BlurDemo::DrawScene()
{
	// Render to our offscreen texture.  Note that we can use the same depth/stencil buffer
	// we normally use since our offscreen texture matches the dimensions.  

	ID3D11RenderTargetView* renderTargets[1] = { _offscreenRTV.Get()};
	_deviceContext->OMSetRenderTargets(1, renderTargets, _depthStencilView.Get());

    // 우선 _offscreenRTV에 그린다.
	_deviceContext->ClearRenderTargetView(_offscreenRTV.Get(), reinterpret_cast<const float*>(&Colors::Silver));
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//
	// Draw the scene to the offscreen texture
	//

	DrawWrapper();

	//
	// Restore the back buffer.  The offscreen render target will serve as an input into
	// the compute shader for blurring, so we must unbind it from the OM stage before we
	// can use it as an input into the compute shader.
	//

    // 다시 RenderTarget으로 바꾸고
	renderTargets[0] = _renderTargetView.Get();
	_deviceContext->OMSetRenderTargets(1, renderTargets, _depthStencilView.Get());

	//mBlur.SetGaussianWeights(4.0f);

    // Gaussian Blur 적용
	_blur.BlurInPlace(_deviceContext, _offscreenSRV, _offscreenUAV, 4);

	//
	// Draw fullscreen quad with texture of blurred scene on it.
	//

	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Silver));
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DrawScreenQuad();

	HR(_swapChain->Present(0, 0));
}
```

```cpp
void BlurFilter::BlurInPlace(ComPtr<ID3D11DeviceContext> dc,
	ComPtr<ID3D11ShaderResourceView> inputSRV,
	ComPtr<ID3D11UnorderedAccessView> inputUAV,
	int32 blurCount)
{
	//
	// Run the compute shader to blur the offscreen texture.
	// 

	for (int i = 0; i < blurCount; ++i)
	{
		// HORIZONTAL blur pass.
		D3DX11_TECHNIQUE_DESC techDesc;
		Effects::BlurFX->HorzBlurTech->GetDesc(&techDesc);
		for (uint32 p = 0; p < techDesc.Passes; ++p)
		{
			Effects::BlurFX->SetInputMap(inputSRV);
			Effects::BlurFX->SetOutputMap(_blurredOutputTexUAV);
			Effects::BlurFX->HorzBlurTech->GetPassByIndex(p)->Apply(0, dc.Get());

			// How many groups do we need to dispatch to cover a row of pixels, where each
			// group covers 256 pixels (the 256 is defined in the ComputeShader).
			uint32 numGroupsX = (uint32)ceilf(_width / 256.0f);
			dc->Dispatch(numGroupsX, _height, 1);
		}


		// Unbind the input texture from the CS for good housekeeping.
		ID3D11ShaderResourceView* nullSRV[1] = { 0 };
		dc->CSSetShaderResources(0, 1, nullSRV);

		// Unbind output from compute shader (we are going to use this output as an input in the next pass, 
		// and a resource cannot be both an output and input at the same time.
		ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
		dc->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

		// VERTICAL blur pass.
		Effects::BlurFX->VertBlurTech->GetDesc(&techDesc);
		for (uint32 p = 0; p < techDesc.Passes; ++p)
		{
			Effects::BlurFX->SetInputMap(_blurredOutputTexSRV);
			Effects::BlurFX->SetOutputMap(inputUAV);
			Effects::BlurFX->VertBlurTech->GetPassByIndex(p)->Apply(0, dc.Get());

			// How many groups do we need to dispatch to cover a column of pixels, where each
			// group covers 256 pixels  (the 256 is defined in the ComputeShader).
			uint32 numGroupsY = (uint32)ceilf(_height / 256.0f);
			dc->Dispatch(_width, numGroupsY, 1);
		}

		dc->CSSetShaderResources(0, 1, nullSRV);
		dc->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	}

	// Disable compute shader.
	dc->CSSetShader(0, 0, 0);
}
```