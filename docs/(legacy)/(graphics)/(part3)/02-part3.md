---
layout: default
title: "02. Picking - CPU에서 Picking처리하기"
parent: "(Rendering Techniques)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/2/2.picking)

* (목표) 물제체 마우스 오버, 클릭을 처리해 보자.
    * 대충 생각나는 방법으론
    * 1. 카메라 방향으로 Ray를 쏘아서 Picking여부를 체크(느리다, GPU메모리를 읽어 CPU에서 처리)
    * 2. 화면을 캡쳐해 Picking여부를 체크(CPU메모리에 저장되기에 그나마 빠르다) - **이걸 사용**

* 어떻게 화면을 캡쳐해 물체를 인식할까?
    * Rendertarget을 두 개 사용하여 캡쳐용, 실제 랜더링용으로 구분한다.
    * 캡쳐용은 Object별로 특수한 색을 반영하여 마우스 클릭시 해당 Object인지 인식한다.
    * 말로 설명하면 어려우니 그림으로 보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-2-1.png"/>
</p>

* 마우스 위의 색이 빨강일시 물체를 체크한걸로 생각하면 되겠지?

## 화면 캡쳐하기

```cpp
// 일단 캡쳐는 이런식으로 한다.
if (m_captureFlag) {

    m_captureFlag = false;

    ComPtr<ID3D11Texture2D> backBuffer;
    // backBuffer로 현재 SwapChain의 Backbuffer를 복사한다.
    m_swapChain->GetBuffer(0,
                            IID_PPV_ARGS(backBuffer.GetAddressOf()));
    // m_tempTexture에 backBuffer를 넣는다
        // ComPtr<ID3D11Texture2D> m_tempTexture;
        // 참고로 ResolveSubresource는 GPU메모리에서 GPU메모리로 복사하는 함수이다
    m_context->ResolveSubresource(m_tempTexture.Get(), 0,
                                    backBuffer.Get(), 0,
                                    DXGI_FORMAT_R8G8B8A8_UNORM);

    // 아니 이게 무슨 뻘짓인가? 그냥 backBuffer를 바로 저장해 버리면 되지?
        // backBuffer는 SwapChain의 BackBuffer이다.
        // 당연히 DXGI_FORMAT_R8G8B8A8_UNORM 포맷이 아니라 포맷 변환이 필요하며,
        // GPU의 최적화를 위해 GPU메모리를 사용할때 특정 목적에 맞게 만들어야한다.
        // Ex) 랜더링용 Texture 메모리는 렌더링용으로만 쓴다든지 등.
    D3D11Utils::WriteToFile(m_device, m_context, m_tempTexture,
                            "captured.png");
}
```

```cpp
void D3D11Utils::WriteToFile(ComPtr<ID3D11Device> &device,
                             ComPtr<ID3D11DeviceContext> &context,
                             ComPtr<ID3D11Texture2D> &textureToWrite,
                             const std::string filename) {

    D3D11_TEXTURE2D_DESC desc;
    textureToWrite->GetDesc(&desc);
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // CPU에서 읽기 가능
    desc.Usage = D3D11_USAGE_STAGING; // GPU에서 CPU로 보낼 데이터를 임시 보관

    ComPtr<ID3D11Texture2D> stagingTexture;
    if (FAILED(device->CreateTexture2D(&desc, nullptr,
                                       stagingTexture.GetAddressOf()))) {
        cout << "Failed()" << endl;
    }

    // 참고: 전체 복사할 때
    // context->CopyResource(stagingTexture.Get(), pTemp.Get());

    // 일부만 복사할 때 사용
    D3D11_BOX box;
    box.left = 0;
    box.right = desc.Width;
    box.top = 0;
    box.bottom = desc.Height;
    box.front = 0;
    box.back = 1;
    context->CopySubresourceRegion(stagingTexture.Get(), 0, 0, 0, 0,
                                   textureToWrite.Get(), 0, &box);

    // R8G8B8A8 이라고 가정
    std::vector<uint8_t> pixels(desc.Width * desc.Height * 4);

    // ** GPU메모리에서 CPU메모리로 복사 **
    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(stagingTexture.Get(), NULL, D3D11_MAP_READ, NULL,
                 &ms); // D3D11_MAP_READ 주의

    // 텍스춰가 작을 경우에는
    // ms.RowPitch가 width * sizeof(uint8_t) * 4보다 클 수도 있어서
    // for문으로 가로줄 하나씩 복사
    uint8_t *pData = (uint8_t *)ms.pData;
    for (unsigned int h = 0; h < desc.Height; h++) {
        memcpy(&pixels[h * desc.Width * 4], &pData[h * ms.RowPitch],
               desc.Width * sizeof(uint8_t) * 4);
    }

    context->Unmap(stagingTexture.Get(), NULL);

    stbi_write_png(filename.c_str(), desc.Width, desc.Height, 4, pixels.data(),
                   desc.Width * 4);

    cout << filename << endl;
}
```

---

## picking여부 체크

```cpp
// BasicConstantData.h

struct BasicPixelConstantData {
    Vector3 eyeWorld;         // 12
    bool useTexture;          // bool 1 + 3 padding
    Material material;        // 48
    Light lights[MAX_LIGHTS]; // 48 * MAX_LIGHTS

    // Constant데이터에 이제 picking용으로 사용한 indexColor를 추가해준다.
    Vector4 indexColor;       // Picking용
};
```

```cpp
// BasicPixelShader.hlsl

cbuffer BasicPixelConstantData : register(b0)
{
    float3 eyeWorld;
    bool useTexture;
    Material material;
    Light light[MAX_LIGHTS];

    // 쉐이더에도 indexColor를 추가해준다.
    float4 indexColor; // 피킹(Picking)에 사용
};

// ...


PixelShaderOutput main(PixelShaderInput input)
{
    // ...

    PixelShaderOutput output;
    output.pixelColor = diffuse + specular;

    // indexColor를 넘겨준 indexColor로 넣어준다
    output.indexColor = indexColor
    
    return output;
}
```

```cpp
void ExampleApp::Render() {

    // ...

    // Picking
    { 
        // m_indexTempTexture에서 m_indexTexture로 넣는다
        m_context->ResolveSubresource(m_indexTempTexture.Get(), 0,
                                      m_indexTexture.Get(), 0,
                                      DXGI_FORMAT_B8G8R8A8_UNORM);

        // 일부만 복사할 때 사용
            // 마우스 커서위의 색상만 복사하면 되니 이렇게 사용
        D3D11_BOX box;
        box.left = m_cursorX;
        box.right = m_cursorX + 1;
        box.top = m_cursorY;
        box.bottom = m_cursorY + 1;
        box.front = 0;
        box.back = 1;
        m_context->CopySubresourceRegion(m_indexStagingTexture.Get(), 0, 0, 0, 0,
                                       m_indexTempTexture.Get(), 0, &box);

        D3D11_MAPPED_SUBRESOURCE ms;
        m_context->Map(m_indexStagingTexture.Get(), NULL, D3D11_MAP_READ, NULL,
                     &ms); // D3D11_MAP_READ 주의

        memcpy(m_pickColor, ms.pData, sizeof(uint8_t) * 4);

        m_context->Unmap(m_indexStagingTexture.Get(), NULL);

    }
}
```

```cpp
bool ExampleApp::Initialize() {

    // ...
    // 물체 1
    {
        // ...

        // 물체의 초기화시 indexColor를 빨강으로 지정
        m_meshGroupSphere.m_basicPixelConstantData.indexColor =
            Vector4(1.0f, 0.0, 0.0, 0.0);
```

```cpp
void ExampleApp::Update(float dt) {

    // ...

    if (m_pickColor[0] == 255) {
        // 물체가 빨간색이면 빨간색으로 그려달라
        m_meshGroupSphere.m_basicPixelConstantData.material.diffuse =
			Vector3(1.0f, 0.1f, 0.1f);
    } else {
        m_meshGroupSphere.m_basicPixelConstantData.material.diffuse =
            Vector3(0.5f);
    }
```