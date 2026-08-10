---
layout: default
title: "13. hlsl 디버깅 + 간단 써보기"
parent: "(Realtime Pipeline)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌎](https://github.com/Arthur880708/Directx11_2_Rasterization/tree/main/3_hlsl)

## 디버깅

```cpp
void AppBase::CreateVertexShaderAndInputLayout(
    const wstring &filename,
    const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
    ComPtr<ID3D11VertexShader> &vertexShader,
    ComPtr<ID3D11InputLayout> &inputLayout) {

    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    UINT compileFlags = 0;

// 디버그를 위해서 flag를 수정
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    // 주의: 쉐이더의 시작점의 이름이 "main"인 함수로 지정
    HRESULT hr = D3DCompileFromFile(filename.c_str(), 0, 0, "main", "vs_5_0",
                                    compileFlags, 0, &shaderBlob, &errorBlob);
```

* 디버그 -> 그래픽 -> 그래픽 디버깅 시작

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx11/d11-3-1.png"/>
</p>

* 보고자하는 픽셀을 클릭
* 우측에 픽셀기록이 뜬다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx11/d11-3-2.png"/>
</p>

* Vertext쉐이더가 어떻게 돌아갔는지 확인하고 싶다면 Vertext Shader의 재생버튼을 누른다

---

## (TIPS) .cso파일

* 참고) Shader가 빌드되고 나면 .cso파일이 떨어지게 됨.
* 매번 Shader를 빌드하지말고 .cso를 쓰는것도 하나의 방법이 될수 있다.
방법은 cso를 바이너리로 읽어서 넣음. (코드참조)

---

## 실습 (Constant Buffer 써보기)

* [Clone Code 🌎](https://github.com/EasyCoding-7/DirectX11-Examples/tree/4/4_shader)

```cpp
// 주의:

// For a constant buffer (BindFlags of D3D11_BUFFER_DESC set to
// D3D11_BIND_CONSTANT_BUFFER), you must set the ByteWidth value of
// D3D11_BUFFER_DESC in multiples of 16, and less than or equal to
// D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.
// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createbuffer

// Constant Buffer에 넘길땐 16bytes씩 끊어서 보내야 한다.
static_assert((sizeof(ModelViewProjectionConstantBuffer) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");
```

* 하고자 하는 것
    * ui 컨트롤(Constant Buffer)에 따라 색상을 변경해주는 Shader를 만들고 싶다

```cpp
/*
    * 우선 Pixel 쉐이더를 보자면
*/

// ui컨트롤을 받아올 ConstantBuffer
cbuffer PixelShaderConstantBuffer : register(b0) { float xSplit; };

// Texture의 Coordination을 추가
struct PixelShaderInput {
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    // TODO: 버텍스 쉐이더와 맞춰주기 (텍스춰 좌표 추가)
    float2 texcoord : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET {

    // TODO: 텍스춰 좌표를 이용해서 색 결정

    return input.texcoord.x > xSplit ? float4(0.0, 0.0, 1.0, 1.0)
                                     : float4(1.0, 0.0, 0.0, 1.0);

    // Use the interpolated vertex color
    // return float4(input.color, 1.0);
}
```

```cpp
// Vertex Shader는 더 쉽다.
    // texcoordination만 넘겨준다

PixelShaderInput main(VertexShaderInput input) {

    PixelShaderInput output;
    float4 pos = float4(input.pos, 1.0f);

    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.pos = pos;
    output.color = input.color;
    // TODO: 텍스춰 좌표 추가!
    output.texcoord = input.texcoord;

    return output;
}
```

* 자! 이제 Constant Buffer를 어떻게 넘기냐 문제

```cpp
// TODO: 픽셀쉐이더에서 사용할 Constant Buffer Data
ComPtr<ID3D11Buffer> m_pixelShaderConstantBuffer;
PixelShaderConstantBuffer m_pixelShaderConstantBufferData;
```

```cpp
bool ExampleApp::Initialize() {

    // ...

    // TODO: 픽셀쉐이더로 보낼 ConstantBuffer 만들기
    AppBase::CreateConstantBuffer(m_pixelShaderConstantBufferData,
                                  m_pixelShaderConstantBuffer);
```

```cpp
/*
    * 추가로 Texcoordination 정보를 input assembler에 넘겨준다
*/

    vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
             D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
             D3D11_INPUT_PER_VERTEX_DATA, 0},
            // TODO: 텍스춰 좌표를 버텍스 쉐이더로 보내겠다!
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
             D3D11_INPUT_PER_VERTEX_DATA, 0},

            /*
           * 이 순서임을 기억하자 *
           struct Vertex {
              Vector3 position;
              Vector3 color;
              Vector2 texcoord;
           };
            */
        };
```

```cpp
void ExampleApp::Update(float dt) {

    // ...

    // TODO: 픽셀 쉐이더에서 사용할 ConstantBuffer 업데이트
    AppBase::UpdateBuffer(m_pixelShaderConstantBufferData,
                          m_pixelShaderConstantBuffer);
}

void ExampleApp::Render() {

    // ...

    // TODO: 여기서 뭘 해줘야 할까요?
    m_context->PSSetShader(m_colorPixelShader.Get(), 0, 0);

    m_context->PSSetConstantBuffers(0, 1,
                                    m_pixelShaderConstantBuffer.GetAddressOf());
```