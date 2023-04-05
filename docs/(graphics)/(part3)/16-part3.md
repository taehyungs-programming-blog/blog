---
layout: default
title: "16. HDR Pipeline"
parent: "(Rendering Techniques)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/16/16.HDRPipeline)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-16-1.png"/>
</p>

* Scene을 Rendering하여
* MSAA(Multi Sampling Anti Aliasing) 후 Render Target에 그린다
	* MSAA - 한 픽셀에 여러 색이 샘플링 되어 있음.
* FP(Float Point)로 그리는 색상의 해상도를 크게하고
* SDR(LDR) - RGB의 범위를 PF가 아니라 0~1로 변환

---

```cpp
void ExampleApp::Render() {

    // ...

    SetViewport();

    const float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    // m_floatRTV -> 이제 Render Target을 float 형식으로 쓸것이다
    m_context->ClearRenderTargetView(m_floatRTV.Get(), clearColor);
    m_context->ClearDepthStencilView(m_depthStencilView.Get(),
                                     D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                     1.0f, 0);

    // ...

    // MSAA로 Texture2DMS에 렌더링 된 결과를 Texture2D로 변환(Resolve)
        // m_floatBuffer가 MSAA로 Sampling되어 있고 m_resolvedBuffer에 Resolved된 결과를 넣음
    m_context->ResolveSubresource(m_resolvedBuffer.Get(), 0,
                                  m_floatBuffer.Get(), 0,
                                  DXGI_FORMAT_R16G16B16A16_FLOAT);

    m_postProcess.Render(m_context);
}
```

```cpp
bool AppBase::InitDirect3D() {

    // ...

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferDesc.Width = m_screenWidth;
    sd.BufferDesc.Height = m_screenHeight;
    // 최종적으로 Rendering되는 Swapchain에는 DXGI_FORMAT_R8G8B8A8_UNORM 포맷으로 넣는다
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferCount = 2;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_mainWindow;
    sd.Windowed = TRUE;
    sd.Flags =
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
    // DirectX에서 MSAA를 쓴다면 DXGI_SWAP_EFFECT_FLIP_DISCARD를 쓰라고 권고함 but!
    // sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //ImGui 폰트가 두꺼워짐
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.SampleDesc.Count = 1; // _FLIP_은 MSAA 미지원
    sd.SampleDesc.Quality = 0;

    CreateBuffers();

    // ...
```

```cpp
void AppBase::CreateBuffers() {

    // 레스터화 -> float/depthBuffer(MSAA) -> resolved -> backBuffer

    // BackBuffer는 화면으로 최종 출력되기 때문에  RTV만 필요하고 SRV는 불필요
    ComPtr<ID3D11Texture2D> backBuffer;
    ThrowIfFailed(
        m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));
    ThrowIfFailed(m_device->CreateRenderTargetView(
        backBuffer.Get(), NULL, m_backBufferRTV.GetAddressOf()));

    // FLOAT MSAA RenderTargetView/ShaderResourceView
    ThrowIfFailed(m_device->CheckMultisampleQualityLevels(
        DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &m_numQualityLevels));

    D3D11_TEXTURE2D_DESC desc;
    backBuffer->GetDesc(&desc);
    desc.MipLevels = desc.ArraySize = 1;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    // buffer 생성시에는 DXGI_FORMAT_R16G16B16A16_FLOAT 포맷을 사용
    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    desc.Usage = D3D11_USAGE_DEFAULT; // 스테이징 텍스춰로부터 복사 가능
    desc.MiscFlags = 0;
    desc.CPUAccessFlags = 0;
    // MSAA역시 사용
    if (m_useMSAA && m_numQualityLevels) {
        desc.SampleDesc.Count = 4;
        desc.SampleDesc.Quality = m_numQualityLevels - 1;
    } else {
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
    }

    // ...
```

{% endraw %}