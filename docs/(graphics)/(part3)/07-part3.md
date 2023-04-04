---
layout: default
title: "07. 기하 쉐이더(Geometry Shader))"
parent: "(Rendering Techniques)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/7/7.GeometryShader)

## Geometry Shader의 필요성

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-7-2.png"/>
</p>

* 위 그림의 왼쪽 캐릭터와 같이 낮은 해상도를 높은 해상도로 수정하고 싶다며 어떻게 해야할까?
* CPU에서 해상도를 늘리는 것은 그리 효율적인 방법이 아니다. -> GPU에서 처리하는 **Geometry shader사용**

---

## Geometry Shader는 Pipeline의 어디에 위치하나?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-7-1.jpg"/>
</p>

* DirectX PipeLine을 보면 Vertex Shader에서 점을찍어(보통 세 점Polygon) Geometry Shader로 넘기는다(현재는 Hull, Tessellator, Domin Shader가 구현이 되어 있지 않으니 이렇게 볼 수 있음)
* 말인 즉슨 Vertex Shader에서 온 **점**을 바탕으로 Geometry는 어떠한 처리를 한다(늘리든 줄이든?)

---

## 만들어 볼 것!

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-7-3.png"/>
</p>

* Vertex Shader에서 5개의 점 정보를 넘겨주면 Geometry Shader에서 위로 점선을 그어준다

### 들어가기 전 ... 코드의 구조를 간략히 설명하자면 .

```cpp
bool ExampleApp::Initialize() {

    if (!AppBase::Initialize())
        return false;

    // app이 시작하며 billboard의 point를 지정해 준다
    m_billboardPoints.Initialize(m_device, {{-0.5f, 0.3f, 0.0f, 1.0f},
                                            {-0.25f, 0.3f, 0.0f, 1.0f},
                                            {0.0f, 0.3f, 0.0f, 1.0f},
                                            {0.25f, 0.3f, 0.0f, 1.0f},
                                            {0.5f, 0.3f, 0.0f, 1.0f}});
```

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
};
```

```cpp
// Initialize 내부는 다음과 같다.
void BillboardPoints::Initialize(ComPtr<ID3D11Device> &device,
                                 const std::vector<Vector4> &points) {

    // Sampler 만들기
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());

	// point의 개수만큼 Vertex Buffer를 생성하고
    D3D11Utils::CreateVertexBuffer(device, points, m_vertexBuffer);

    m_indexCount = uint32_t(points.size());

    m_constantData.width = 0.1f;
	// Geometry Shader도 Shader이기에 Constant Buffer를 사용이 가능함.
    D3D11Utils::CreateConstantBuffer(device, m_constantData, m_constantBuffer);

    // Geometry shader 초기화하기
    D3D11Utils::CreateGeometryShader(
        device, L"BillboardPointsGeometryShader.hlsl", m_geometryShader);

    vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, // Vector4
         D3D11_INPUT_PER_VERTEX_DATA, 0}};

	// Vertex Shader를 생성하고 input으론 pos정보가 넘어갈 겁니다
    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"BillboardPointsVertexShader.hlsl", inputElements,
        m_vertexShader, m_inputLayout);

    D3D11Utils::CreatePixelShader(device, L"BillboardPointsPixelShader.hlsl",
                                  m_pixelShader);
}
```

```cpp
struct VertexShaderInput
{
    float4 pos : POSITION; // 모델 좌표계의 위치 position
};

struct GeometryShaderInput
{
    float4 pos : SV_POSITION; // Screen position
};

GeometryShaderInput main(VertexShaderInput input)
{
    GeometryShaderInput output;
    
    // Geometry shader로 그대로 넘겨줍니다.
    output.pos = input.pos;
    
    return output;
}
```

```cpp
void BillboardPoints::Render(ComPtr<ID3D11DeviceContext> &context) {

    context->VSSetShader(m_vertexShader.Get(), 0, 0);
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
    context->PSSetShader(m_pixelShader.Get(), 0, 0);

	// Constant buffer를 넘기고
    context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    context->GSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	// 이 shader를 사용할 겁니다
    context->GSSetShader(m_geometryShader.Get(), 0, 0);

    context->IASetInputLayout(m_inputLayout.Get());

    UINT stride = sizeof(Vector4); // sizeof(Vertex);
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride,
                                &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    // 바꿔줄 필요 없이 그대로 POINTLIST 사용
    // context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // POINTLIST는 연결관계가 필요 없기 때문에 DrawIndexed() 대신 Draw() 사용
    context->Draw(m_indexCount, 0);

    // Geometry Shader를 쓰지 않는 다른 물체들을 위해 nullptr로 설정
    context->GSSetShader(nullptr, 0, 0);
}
```

```cpp
void ExampleApp::Update(float dt) {

    // 참고로 Constant Buffer는 여기서 채워준다

    m_billboardPoints.m_constantData.eyeWorld = eyeWorld;
    m_billboardPoints.m_constantData.view = viewRow.Transpose();
    m_billboardPoints.m_constantData.proj = projRow.Transpose();
    D3D11Utils::UpdateBuffer(m_device, m_context,
                             m_billboardPoints.m_constantData,
                             m_billboardPoints.m_constantBuffer);
```

```cpp
struct PixelShaderInput
{
    float4 pos : SV_POSITION; // not POSITION
    uint primID : SV_PrimitiveID;
};

//TODO: PointStream -> TriangleStream
[maxvertexcount(100)] // 최대 출력 Vertex 갯수 (최대 32bit(1024))
void main(point GeometryShaderInput input[1], uint primID : SV_PrimitiveID,
                              inout PointStream<PixelShaderInput> outputStream)
// geometry shader는 return을 inout을 통해 하게 된다.
// 위 예시는 PointStream을 썼는데 Point로 리턴하겠다는 뜻
// TriangleStream으로 inout할시 Triangle이 된다.
{
    
    // float hw = 0.5 * width;
    
    PixelShaderInput output;
    
    output.pos = input[0].pos;
    
    for (int i = 0; i < 100; i ++)
    {
        output.pos = input[0].pos + float4(0.0, 0.003, 0.0, 0.0) * float(i);
        output.pos = mul(output.pos, view);
        output.pos = mul(output.pos, proj);
        output.primID = primID;

        outputStream.Append(output);
    }
    // 주의: GS는 Triangle Strips으로 출력합니다.
    // https://learn.microsoft.com/en-us/windows/win32/direct3d9/triangle-strips

    // outputStream.RestartStrip(); // Strip을 다시 시작
}

```

---

* 해볼 것 -> Vertex를 받아서 네모를 그려보자.
	* 점 하나를 네모로 바꿔보자

```cpp
[maxvertexcount(4)] // 최대 출력 Vertex 갯수
void main(point GeometryShaderInput input[1], uint primID : SV_PrimitiveID,
                              // inout PointStream<PixelShaderInput> outputStream)
                            inout TriangleStream<PixelShaderInput> outputStream)
{
    
    // float hw = 0.5 * width;
    
    PixelShaderInput output;
    
    float hw = 0.5 * width;
    
    output.pos = input[0].pos + float4(-hw, -hw, 0.0f, 0.0f);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.primID = primID;
    
    outputStream.Append(output);
    
    output.pos = input[0].pos + float4(-hw, hw, 0.0f, 0.0f);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.primID = primID;
    
    outputStream.Append(output);
    
    output.pos = input[0].pos + float4(hw, -hw, 0.0f, 0.0f);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.primID = primID;
    
    outputStream.Append(output);
    
    output.pos = input[0].pos + float4(hw, hw, 0.0f, 0.0f);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.primID = primID;
    
    outputStream.Append(output);
}
```