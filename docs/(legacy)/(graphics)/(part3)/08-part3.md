---
layout: default
title: "08. Geometry Shader 활용 - billboards"
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

{% raw %}

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/8/8.billboards)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-8-1.png"/>
</p>

* 3D 게임에서 뒤에보이는 나무와 같은 배경은 어떻게 그릴까?
    * 나무라는 3D Object를 넣는것일까?
    * 비슷하지만 사실 그림 Object(Texture)를 붙인다고 생각하면 쉽다
* 이번에 해볼 것은 그림을 효율적으로 붙이는 방법!

* 이게 Geometry Shader와 무슨상관이지??
* 하고싶은 것은 이것이다 -> 하나의 Geometry Shader에 다른 모양의 Texture를 어떻게 그릴수 있는지?

---

```cpp
class BillboardPoints {
  public:
    void Initialize(ComPtr<ID3D11Device> &device,
                    const std::vector<Vector4> &points);

    void Render(ComPtr<ID3D11DeviceContext> &context);

  public:
    BillboardPointsConstantData m_constantData;

    // 편의상 ConstantBuffer를 하나만 사용
    ComPtr<ID3D11Buffer> m_constantBuffer;

  protected:
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11SamplerState> m_samplerState;
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11GeometryShader> m_geometryShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;

    uint32_t m_indexCount = 0;

    // m_texArray에 들어온 이미지를 Array형식으로 만들어 사용할 예정
    ComPtr<ID3D11Texture2D> m_texArray;
    ComPtr<ID3D11ShaderResourceView> m_texArraySRV;
};
```

```cpp
void BillboardPoints::Initialize(ComPtr<ID3D11Device> &device,
                                 const std::vector<Vector4> &points) {

    // ...

    std::vector<std::string> filenames = {
        "../Assets/Textures/TreeBillboards/1.png",
        "../Assets/Textures/TreeBillboards/2.png",
        "../Assets/Textures/TreeBillboards/3.png",
        "../Assets/Textures/TreeBillboards/4.png",
        "../Assets/Textures/TreeBillboards/5.png"};

    D3D11Utils::CreateTextureArray(device, filenames, m_texArray,
                                   m_texArraySRV);
}
```

```cpp
void D3D11Utils::CreateTextureArray(
    ComPtr<ID3D11Device> &device, const std::vector<std::string> filenames,
    ComPtr<ID3D11Texture2D> &texture,
    ComPtr<ID3D11ShaderResourceView> &textureResourceView) {

    // 모든 이미지의 width와 height가 같다고 가정합니다.

    int width = 0, height = 0;
    std::vector<uint8_t> imageArray;
    for (const auto &f : filenames) {

        cout << f << endl;

        std::vector<uint8_t> image;

        ReadImage(f, image, width, height);

        imageArray.insert(imageArray.begin(), image.begin(), image.end());
    }

    // Create texture.
    D3D11_TEXTURE2D_DESC txtDesc;
    ZeroMemory(&txtDesc, sizeof(txtDesc));
    txtDesc.Width = UINT(width);
    txtDesc.Height = UINT(height);
    txtDesc.MipLevels = 1;
    txtDesc.ArraySize = UINT(filenames.size());
    txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.SampleDesc.Quality = 0;
    txtDesc.Usage = D3D11_USAGE_IMMUTABLE;
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    // SUBRESOURCE_DATA의 배열
    std::vector<D3D11_SUBRESOURCE_DATA> initData(filenames.size());
    size_t offset = 0;
    for (auto &i : initData) {
        i.pSysMem = imageArray.data() + offset;
        i.SysMemPitch = txtDesc.Width * sizeof(uint8_t) * 4;
        i.SysMemSlicePitch = txtDesc.Width * txtDesc.Height * sizeof(uint8_t) * 4;
        offset += i.SysMemSlicePitch;
    }

    device->CreateTexture2D(&txtDesc, initData.data(), texture.GetAddressOf());

    // 리소스를 쉐이더에서 사용하기 위해 Resource View를 만든다
    D3D11_SHADER_RESOURCE_VIEW_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Format = txtDesc.Format;
    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    desc.Texture2DArray.MostDetailedMip = 0;
    desc.Texture2DArray.MipLevels = txtDesc.MipLevels;
    desc.Texture2DArray.FirstArraySlice = 0;
    desc.Texture2DArray.ArraySize = txtDesc.ArraySize;

    device->CreateShaderResourceView(texture.Get(), &desc,
                                     textureResourceView.GetAddressOf());
}
```

```cpp
void BillboardPoints::Render(ComPtr<ID3D11DeviceContext> &context) {

    context->VSSetShader(m_vertexShader.Get(), 0, 0);
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
    context->PSSetShader(m_pixelShader.Get(), 0, 0);

    // 마지막으로 렌더링시 이렇게 Texture Resource View를 넣어준다
    context->PSSetShaderResources(0, 1, m_texArraySRV.GetAddressOf());
```

```cpp
struct PixelShaderInput
{
    float4 pos : SV_POSITION; // not POSITION
    // 텍스춰 좌표 추가된것을 확인가능
    float2 texCoord : TEXCOORD; 
    uint primID : SV_PrimitiveID;
};

[maxvertexcount(4)]
void main(point GeometryShaderInput input[1], uint primID : SV_PrimitiveID,
                              inout TriangleStream<PixelShaderInput> outputStream)
{
    float hw = 0.5 * width;
    
    PixelShaderInput output;
    
    output.pos = input[0].pos + float4(-hw, -hw, 0.0, 0.0);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(0.0, 1.0);
    output.primID = primID;
    
    outputStream.Append(output);

    output.pos = input[0].pos + float4(-hw, hw, 0.0f, 0.0f);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(0.0, 0.0);
    output.primID = primID;
    
    outputStream.Append(output);
    
    output.pos = input[0].pos + float4(hw, -hw, 0.0f, 0.0f);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(1.0, 1.0);
    output.primID = primID;
    
    outputStream.Append(output);
    
    output.pos = input[0].pos + float4(hw, hw, 0.0f, 0.0f);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(1.0, 0.0);
    output.primID = primID;
    
    outputStream.Append(output);

    // outputStream.RestartStrip();
}
```

```cpp
PixelShaderOutput main(PixelShaderInput input)
{
    float3 uvw = float3(input.texCoord, float(input.primID % 5));
    float4 pixelColor = g_texArray.Sample(g_sampler, uvw);

    // clip(x)에서 x가 0보다 작으면 이 픽셀의 색은 버린다.     
    
    // alpha 값이 있는 이미지에서 불투명도가 0.9보다 작으면 clip
    //clip(pixelColor.a - 0.9f);
    
    // 픽셀의 값이 흰색에 가까운 배경 색이면 clip
    //TODO: clip(...)
    //float i = (pixelColor.r + pixelColor.g + pixelColor.b) / 3.0;
    //clip((i > 0.8) ? -1 : 1);
    
    clip((pixelColor.a < 0.9f) || (pixelColor.r + pixelColor.g + pixelColor.b) > 2.4 ? -1 : 1);
    
    PixelShaderOutput output;
    
    output.pixelColor = pixelColor;

    return output;
}

```

---

* 나를 따라 나무가 회전하게 만들어 보자.

```cpp
[maxvertexcount(4)]
void main(point GeometryShaderInput input[1], uint primID : SV_PrimitiveID,
                              inout TriangleStream<PixelShaderInput> outputStream)
{
    float hw = 0.5 * width;
    
    float4 up = float4(0.0, 1.0, 0.0, 0.0);
    float4 front = float4(eyeWorld, 1.0) - input[0].pos;
    front.w = 0.0;
    float4 right = float4(cross(up.xyz, normalize(front.xyz)), 0.0);
    
    
    PixelShaderInput output;
    
    output.pos = input[0].pos - hw * right - hw * up;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(0.0, 1.0);
    output.primID = primID;
    
    outputStream.Append(output);

    output.pos = input[0].pos - hw * right + hw * up;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(0.0, 0.0);
    output.primID = primID;
    
    outputStream.Append(output);
    
    output.pos = input[0].pos + hw * right - hw * up;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(1.0, 1.0);
    output.primID = primID;
    
    outputStream.Append(output);
    
    output.pos = input[0].pos + hw * right + hw * up;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texCoord = float2(1.0, 0.0);
    output.primID = primID;
    
    outputStream.Append(output);

    // outputStream.RestartStrip();
}
```

{% endraw %}