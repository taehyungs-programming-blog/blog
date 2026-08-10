---
layout: default
title: "28. Image Filter(Bloom Filter)"
parent: "(Realtime Pipeline)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌎](https://github.com/Arthur880708/Directx11_2_Rasterization/tree/main/20_bloomFilter)

## Filter만들기 전 주의사항

```cpp
void Initialize(ComPtr<ID3D11Device> &device,
                ComPtr<ID3D11DeviceContext> &context,
                const wstring vertexPrefix, const wstring pixelPrefix,
                int width, int height) {

    // ...

    // Initialize시에 자신이 원하는 Shader를 선택한다.
    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, vertexPrefix + L"VertexShader.hlsl", basicInputElements,
        m_vertexShader, m_inputLayout);

    D3D11Utils::CreatePixelShader(device, pixelPrefix + L"PixelShader.hlsl",
                                    m_pixelShader);

    // Texture sampler
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    // Sampler의 경우 WRAP이 아니라 CRAMP를 사용하는데 주의
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the Sample State
    device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());


    // Create a rasterizer state
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = false;
    
    // 이미지 후 처리기에 Depth Buffer또한 필요없기에 Enable을 끈다
    rastDesc.DepthClipEnable = false;

    device->CreateRasterizerState(&rastDesc,
                                m_rasterizerSate.GetAddressOf());

    // Set the viewport
    // 경우에 따라 Viewport를 변경해 가며 Filter를 걸수 있기에 Viewport도 별도로 지정한다
    ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.Width = float(width);
    m_viewport.Height = float(height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;


    // 이미지 처리용 texture를 새로 생성한다
    ComPtr<ID3D11Texture2D> texture;

    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = txtDesc.ArraySize = 1;

    //  이미지 처리용도, 일반적 Format보다 사이즈가 훨씬크다
    txtDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; 
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_DEFAULT;

    // 이미지 처리용 Texture는 읽어오고 쓰고 모두가 가능해야함
        // D3D11_BIND_SHADER_RESOURCE - 읽어올수 있다
        // D3D11_BIND_RENDER_TARGET - 쓸수 있다
        // D3D11_BIND_UNORDERED_ACCESS - 연산도 가능
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE |
                        D3D11_BIND_RENDER_TARGET |
                        D3D11_BIND_UNORDERED_ACCESS;

    // ...

    // Texture를 만들고
    device->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf());

    // 그려질 Texture로 선정
    device->CreateRenderTargetView(texture.Get(), &viewDesc,
                                m_renderTargetView.GetAddressOf());

    // 그릴 Texture로 선정(잘보면 그려질, 그릴 texture는 모두 동일함)
    device->CreateShaderResourceView(texture.Get(), nullptr,
                                    m_shaderResourceView.GetAddressOf());
```

사진

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx11/d11-18-1.jpg"/>
</p>

```cpp
void Render(ComPtr<ID3D11DeviceContext> &context) {

        assert(m_shaderResources.size() > 0);
        assert(m_renderTargets.size() > 0);

        // 어디에 렌더링 할지를 지정
        context->OMSetRenderTargets(UINT(m_renderTargets.size()),
                                    m_renderTargets.data(), nullptr);
        //float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        //context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
        context->RSSetState(m_rasterizerSate.Get());
        context->RSSetViewports(1, &m_viewport);

        UINT stride = sizeof(Vertex);
        UINT offset = 0;

        context->IASetInputLayout(m_inputLayout.Get());
        context->IASetVertexBuffers(0, 1, m_mesh->vertexBuffer.GetAddressOf(),
                                    &stride, &offset);
        context->IASetIndexBuffer(m_mesh->indexBuffer.Get(),
                                  DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->VSSetShader(m_vertexShader.Get(), 0, 0);
        context->PSSetShader(m_pixelShader.Get(), 0, 0);
        context->PSSetShaderResources(0, UINT(m_shaderResources.size()),
                                      m_shaderResources.data());
        context->PSSetConstantBuffers(
            0, 1, m_mesh->pixelConstantBuffer.GetAddressOf());
        context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
        context->DrawIndexed(m_mesh->m_indexCount, 0, 0);
    }
```

```cpp
bool AppBase::InitDirect3D() {

    // ...

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferDesc.Width = m_screenWidth;   // set the back buffer width
    sd.BufferDesc.Height = m_screenHeight; // set the back buffer height
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
    sd.BufferCount = 2;                                // Double-buffering
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;

    // Swap Chain 만들시 BackBuffer를 다시 입력으로 넣어서 Filter를 걸어줘야함.
        // DXGI_USAGE_SHADER_INPUT 쉐이더에 입력으로 넣어주기 위해 필요
    sd.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
```

```cpp
bool AppBase::CreateRenderTargetView() {

    ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    if (backBuffer) {

        // 기존에는 RenderTarget만 만드는데
        m_device->CreateRenderTargetView(backBuffer.Get(), nullptr,
                                         m_renderTargetView.GetAddressOf());

        // m_device->CreateShaderResourceView(backBuffer.Get(), nullptr,
        // m_shaderResourceView.GetAddressOf());

        D3D11_TEXTURE2D_DESC desc;
        backBuffer->GetDesc(&desc);
        // 디버깅용
        // cout << desc.Width << " " << desc.Height << " " << desc.Format <<
        // endl;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.MiscFlags = 0;

        if (FAILED(m_device->CreateTexture2D(&desc, nullptr,
                                             m_tempTexture.GetAddressOf()))) {
            cout << "Failed()" << endl;
        }

        // 이젠 Back Buffer를 다시 써야하기에 ShaderResourceView까지 다시 만듦
            // ShaderResource를 (backBuffer가 아니라) tempTexture로부터 생성
        m_device->CreateShaderResourceView(m_tempTexture.Get(), nullptr,
                                           m_shaderResourceView.GetAddressOf());
    } else {
        std::cout << "CreateRenderTargetView() failed." << std::endl;
        return false;
    }

    return true;
}
```

---

## 이제 Filter를 만들어보자

### 자신을 복사하는 Filter

```cpp
void ExampleApp::BuildFilters() {

    m_filters.clear();

    // 자기자신을 복사한는 Filter
    auto copyFilter =
    make_shared<ImageFilter>(m_device, m_context, L"Sampling", L"Sampling",
                                m_screenWidth, m_screenHeight);
    copyFilter->SetShaderResources({m_shaderResourceView});
    m_filters.push_back(copyFilter);

    // 이런식으로 Filter를 하나더 거치게 할 수 있음
    auto finalFilter =
        make_shared<ImageFilter>(m_device, m_context, L"Sampling", L"Sampling",
                                 m_screenWidth, m_screenHeight);
    finalFilter->SetShaderResources({m_filters.back()->m_shaderResourceView});

    // 마지막은 화면의 renderTarget에 그려달라
    finalFilter->SetRenderTargets({this->m_renderTargetView});
    m_filters.push_back(finalFilter);
}
```

```cpp
// Sampling Pixel Shader

float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
    // bloom effect
    float3 color = g_texture0.Sample(g_sampler, input.texcoord).rgb;
    float l = (color.x + color.y + color.y) / 3;
    
    if (l > threshold)
    {
        return float4(color, 1.0f);
    }
    else
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }
}
```

```cpp
// Sampling Vertex Shader

SamplingPixelShaderInput main(SamplingVertexShaderInput input)
{
    SamplingPixelShaderInput output;
    
    output.position = float4(input.position, 1.0);
    output.texcoord = input.texcoord;

    return output;
}

```

---

### Bloom Filter 넣기

```cpp
void ExampleApp::BuildFilters() {

    m_filters.clear();

    // 화면을 한 번 복사한다
    auto copyFilter =
        make_shared<ImageFilter>(m_device, m_context, L"Sampling", L"Sampling",
                                 m_screenWidth, m_screenHeight);
    copyFilter->SetShaderResources({m_shaderResourceView});
    m_filters.push_back(copyFilter);

    // down sampling 한다(해상도를 줄인인다)
    auto downFilter = make_shared<ImageFilter>(
        m_device, m_context, L"Sampling", L"Sampling", m_screenWidth / m_down,
        m_screenHeight / m_down);
    downFilter->SetShaderResources({m_shaderResourceView});
    downFilter->m_pixelConstData.threshold = m_threshold;
    downFilter->UpdateConstantBuffers(m_device, m_context);
    m_filters.push_back(downFilter);

    for (int i = 0; i < m_repeat; i++) {
        // x, y gaussian effect을 넣는다
        auto &prevResource = m_filters.back()->m_shaderResourceView;
        m_filters.push_back(make_shared<ImageFilter>(
            m_device, m_context, L"Sampling", L"BlurX", m_screenWidth / m_down,
            m_screenHeight / m_down));
        m_filters.back()->SetShaderResources({prevResource});

        auto &prevResource2 = m_filters.back()->m_shaderResourceView;
        m_filters.push_back(make_shared<ImageFilter>(
            m_device, m_context, L"Sampling", L"BlurY", m_screenWidth / m_down,
            m_screenHeight / m_down));
        m_filters.back()->SetShaderResources({prevResource2});
    }

    // 다시 해상도를 돌린다(Up Sampling)
    auto upFilter =
        make_shared<ImageFilter>(m_device, m_context, L"Sampling", L"Sampling",
                                 m_screenWidth, m_screenHeight);
    upFilter->SetShaderResources({m_filters.back()->m_shaderResourceView});
    m_filters.push_back(upFilter);

    auto combineFilter =
        make_shared<ImageFilter>(m_device, m_context, L"Sampling", L"Combine",
                                 m_screenWidth, m_screenHeight);
    combineFilter->SetShaderResources({copyFilter->m_shaderResourceView,
                                       m_filters.back()->m_shaderResourceView});
    combineFilter->SetRenderTargets(
        {this->m_renderTargetView}); // 렌더타겟 교체
    combineFilter->m_pixelConstData.strength = m_strength;
    combineFilter->UpdateConstantBuffers(m_device, m_context);
    m_filters.push_back(combineFilter);
}
```