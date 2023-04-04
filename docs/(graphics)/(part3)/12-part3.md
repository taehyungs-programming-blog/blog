---
layout: default
title: "12. mipmap"
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

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/12/12.mipmap)

* 근본적 질문 - mipmap을 어디에 쓸까?
    * 물체가 멀리 있을때는 물체의 해상도를 낮추고 가까이 있을때는 물체의 해상도를 높이는 것이 좋다
    * LOD(Level of detail) 에 활용

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-12-1.png"/>
</p>

* 따라서 먼 곳의 이미지는 해상도가 낮은 텍스쳐를 넣는데 이런 기법을 LOD이라 한다.
- 그냥 높은 해상도로 넣어버리면 되지 않나 싶지만.. 오히려 앨리어싱 현상으로 보기가 더 안좋아진다

* 참고 min, mag) mag(magnification) - Texture를 실제보다 확대
*  min(minification) - Texture를 실제보다 축소


```cpp
void BasicMeshGroup::Initialize(ComPtr<ID3D11Device> &device,
                                ComPtr<ID3D11DeviceContext> &context,
                                const std::vector<MeshData> &meshes) {

    // Sampler 만들기
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    // 지금까지 사용하던 Sampler는 D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR 이다.
    // D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR
        // MIN, MAP을 한 이후 적당힌 interpolation을 해서 동작
        // 만약 LINEAR가 아니라 POINT라면 뚝뚝 끊어져 보이게 된다.
    sampDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;

    // ...
```

---

## Direct3D 11 Graphics Subresources


* [참고사이트](https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-subresources)

* 전체를 하나의 resource라 부르며, 그 중 선택된 부분을 subresource라 부른다
* subresource는 아래와 같이 정의할 수 있다.

* 세로는 mipmap이 된 하나의 Texture의 mipmap이다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-12-2.png"/>
</p>

* 세로 한 줄을 Array Slice라 하며

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-12-3.png"/>
</p>

* 가로 한 줄을 Mip Slice라 한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-12-4.png"/>
</p>

* 이런식으로 각각 Texture를 선택할 수 있고

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-12-5.png"/>
</p>

* Texture를 그룹지어 선택도 가능하다

* 이런식으로 메모리 접근이 가능하단말.

---

* 그래서 mipmap을 어디서 만드냐?

```cpp
// Texture를 만들며 사용된다.

void D3D11Utils::CreateTexture(
    ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
    const std::string filename, ComPtr<ID3D11Texture2D> &texture,
    ComPtr<ID3D11ShaderResourceView> &textureResourceView) {

    int width, height;
    std::vector<uint8_t> image;

    ReadImage(filename, image, width, height);

    // 스테이징 텍스춰 만들고 CPU에서 이미지를 복사합니다.
    ComPtr<ID3D11Texture2D> stagingTexture =
        CreateStagingTexture(device, context, width, height, image);

    // 실제로 사용할 텍스춰 설정
    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = 0; // 밉맵 레벨 최대
    txtDesc.ArraySize = 1;
    txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_DEFAULT; // 스테이징 텍스춰로부터 복사 가능
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // 밉맵 사용
    txtDesc.CPUAccessFlags = 0;

    // 초기 데이터 없이 텍스춰 생성 (전부 검은색)
    device->CreateTexture2D(&txtDesc, nullptr, texture.GetAddressOf());

    // 실제로 생성된 MipLevels를 확인해보고 싶을 경우
    // texture->GetDesc(&txtDesc);
    // cout << txtDesc.MipLevels << endl;

    // 스테이징 텍스춰로부터 가장 해상도가 높은 이미지 복사
    context->CopySubresourceRegion(texture.Get(), 0, 0, 0, 0,
                                   stagingTexture.Get(), 0, nullptr);

    // ResourceView 만들기
    device->CreateShaderResourceView(texture.Get(), 0,
                                     textureResourceView.GetAddressOf());

    // 해상도를 낮춰가며 밉맵 생성
        // DirectX에서 알아서 mipmap을 작성해 준다는 말
    context->GenerateMips(textureResourceView.Get());

    // HLSL 쉐이더 안에서는 SampleLevel() 사용
}
```

```cpp
ComPtr<ID3D11Texture2D>
CreateStagingTexture(ComPtr<ID3D11Device> &device,
                     ComPtr<ID3D11DeviceContext> &context, const int width,
                     const int height, const std::vector<uint8_t> &image,
                     const int mipLevels = 1, const int arraySize = 1) {

    // 스테이징 텍스춰 만들기
    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = mipLevels;
    txtDesc.ArraySize = arraySize;
    txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_STAGING;
    txtDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

    ComPtr<ID3D11Texture2D> stagingTexture;
    if (FAILED(device->CreateTexture2D(&txtDesc, nullptr,
                                       stagingTexture.GetAddressOf()))) {
        cout << "Failed()" << endl;
    }

    // CPU에서 이미지 데이터 복사
    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
    uint8_t *pData = (uint8_t *)ms.pData;
    for (UINT h = 0; h < UINT(height); h++) { // 가로줄 한 줄씩 복사
        memcpy(&pData[h * ms.RowPitch], &image[h * width * 4],
               width * sizeof(uint8_t) * 4);
    }
    context->Unmap(stagingTexture.Get(), NULL);

    return stagingTexture;
}
```

```cpp
PixelShaderOutput main(PixelShaderInput input)
{
    // ...
    
    if (useTexture)
    {
        // diffuse *= g_texture0.Sample(g_sampler, input.texcoord);
        // diffuse *= g_texture0.SampleLevel(g_sampler, input.texcoord, mipmapLevel);
        
        float dist = length(eyeWorld - input.posWorld);
        float distMin = 5.0;
        float distMax = 10.0;
        float lod = 10.0 * saturate(dist / (distMax - distMin));
        
        diffuse *= g_texture0.SampleLevel(g_sampler, input.texcoord, lod);

        // Specular texture를 별도로 사용할 수도 있습니다.
    }

    PixelShaderOutput output;
    output.pixelColor = diffuse + specular;
    output.indexColor = indexColor;
    
    return output;
}

```

{% endraw %}