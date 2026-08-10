---
layout: default
title: "12. MVP(Model View Projection)"
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

* [Clone Code 🌎](https://github.com/Arthur880708/Directx11_2_Rasterization/tree/main/2_MVP)

```cpp
void ExampleApp::Render() {

    // ...

    // Render의 과정에서 ConstantBuffer를 넘기게 된다.
    m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
```

* 대략 Constant Buffer에 넣는 방법은 아래와 같음

```cpp
// 이 예제에서 ConstantBuffer로 보낼 데이터
struct ModelViewProjectionConstantBuffer {
    Matrix model;
    Matrix view;
    Matrix projection;
};
```

```cpp
template <typename T_DATA>
void UpdateBuffer(const T_DATA &bufferData, ComPtr<ID3D11Buffer> &buffer) {
    D3D11_MAPPED_SUBRESOURCE ms;
    m_context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, &bufferData, sizeof(bufferData));
    m_context->Unmap(buffer.Get(), NULL);
}
```

---

* 그래서 오늘 하고자 하는것은?

* ConstantBuffer에 넣을 Model, View, Projection을 어떻게 넣는지, 왜 넣는지??

```cpp
void ExampleApp::Update(float dt) {

    using namespace DirectX;

    // 모델의 변환
    m_constantBufferData.model =
        Matrix::CreateScale(m_modelScaling) * Matrix::CreateRotationY(m_modelRotation.y) *
        Matrix::CreateRotationX(m_modelRotation.x) * Matrix::CreateRotationZ(m_modelRotation.z) *
        Matrix::CreateTranslation(m_modelTranslation);
    m_constantBufferData.model = m_constantBufferData.model.Transpose();

    // 시점 변환
    // m_constantBufferData.view = XMMatrixLookAtLH(m_viewEye, m_viewFocus, m_viewUp);
    m_constantBufferData.view = XMMatrixLookToLH(m_viewEyePos, m_viewEyeDir, m_viewUp);
    m_constantBufferData.view = m_constantBufferData.view.Transpose();

    // 프로젝션
    // m_aspect = AppBase::GetAspectRatio(); // <- GUI에서 조절
    if (m_usePerspectiveProjection) {
        m_constantBufferData.projection = XMMatrixPerspectiveFovLH(
            XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ);
    } else {
        m_constantBufferData.projection =
            XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
    }
    m_constantBufferData.projection = m_constantBufferData.projection.Transpose();

    // Constant를 CPU에서 GPU로 복사
    AppBase::UpdateBuffer(m_constantBufferData, m_constantBuffer);
}
```

---

## Model

* 사실상 Model이 제일쉽다.
    * SRT(Scale, Rotation, Translation)을 적용

```cpp
// 모델의 변환
m_constantBufferData.model =
    Matrix::CreateScale(m_modelScaling) * Matrix::CreateRotationY(m_modelRotation.y) *
    Matrix::CreateRotationX(m_modelRotation.x) * Matrix::CreateRotationZ(m_modelRotation.z) *
    Matrix::CreateTranslation(m_modelTranslation);
// (TIPS) 항상 Transpose를 사용하는데 SimpleMath에선 Row Major를 쓰고 Shader는 Column Major를 쓰기 때문이다.
m_constantBufferData.model = m_constantBufferData.model.Transpose();
```

---

## View

* 카메라의 위치를 지정한다

```cpp
// 시점 변환
    // 카메라의 위치를 말함
// m_constantBufferData.view = XMMatrixLookAtLH(m_viewEye, m_viewFocus, m_viewUp);
m_constantBufferData.view = XMMatrixLookToLH(m_viewEyePos, m_viewEyeDir, m_viewUp);
m_constantBufferData.view = m_constantBufferData.view.Transpose();
```

---

## Projection

* 카메라로 부터 찍힌 화면을 정의한다

```cpp
// 프로젝션
// m_aspect = AppBase::GetAspectRatio(); // <- GUI에서 조절
if (m_usePerspectiveProjection) {
    // Fov(Field Of View) - 시야각을 의미
    m_constantBufferData.projection = XMMatrixPerspectiveFovLH(
        XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ, m_farZ);
} else {
    m_constantBufferData.projection =
        XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
}
m_constantBufferData.projection = m_constantBufferData.projection.Transpose();
```