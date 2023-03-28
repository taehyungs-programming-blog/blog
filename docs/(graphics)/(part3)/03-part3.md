---
layout: default
title: "03. Picking - Intersects"
parent: "(Rendering Techniques)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/3/3.picking-shader)

* 이번엔 DirectXTK의 Intersects함수를 사용해 hit여부를 체크한다
    * 주의할 점은 Near ~ Far 사이의 Object만 Hit해야한다는 점이다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-3-1.png"/>
</p>

```cpp
bool ExampleApp::Initialize() {

    // ...

    // Main Sphere
    {
        Vector3 center(0.0f, 0.0f, 3.0f);
        float radius = 1.0f;
        MeshData sphere = GeometryGenerator::MakeSphere(radius, 100, 100);
        sphere.textureFilename = "../Assets/Textures/earth.jpg";
        m_mainSphere.Initialize(m_device, {sphere});
        m_mainSphere.m_diffuseResView = m_cubeMapping.m_diffuseResView;
        m_mainSphere.m_specularResView = m_cubeMapping.m_specularResView;
        Matrix modelMat = Matrix::CreateTranslation(center);
        Matrix invTransposeRow = modelMat;
        invTransposeRow.Translation(Vector3(0.0f));
        invTransposeRow = invTransposeRow.Invert().Transpose();
        m_mainSphere.m_basicVertexConstantData.model = modelMat.Transpose();
        m_mainSphere.m_basicVertexConstantData.invTranspose =
            invTransposeRow.Transpose();
        m_mainSphere.m_basicPixelConstantData.useTexture = true;
        m_mainSphere.m_basicPixelConstantData.material.diffuse = Vector3(1.0f);
        m_mainSphere.m_basicPixelConstantData.material.specular = Vector3(0.3f);
        m_mainSphere.m_basicPixelConstantData.indexColor =
            Vector4(1.0f, 0.0, 0.0, 0.0);
        m_mainSphere.UpdateConstantBuffers(m_device, m_context);

        // hit 시킬 object와 동일한 크기와 위치에 BoundingSphere 만든다
        m_mainBoundingSphere = BoundingSphere(center, radius);
    }

    // ...
```

* 이제 BoundingSphere와 Ray의 Hit을 체크하면 된다.

```cpp
void ExampleApp::Update(float dt) {

    // ...

    // 마우스 클릭했을 때만 업데이트
    if (m_leftButton) {

        // ViewFrustum에서 가까운 면 위의 커서 위치
        // ViewFrustum에서 먼 면의 커서 위치
        Vector3 cursorNdcNear = Vector3(m_cursorNdcX, m_cursorNdcY, 0.0f);
        Vector3 cursorNdcFar = Vector3(m_cursorNdcX, m_cursorNdcY, 1.0f);

        // NDC 커서 위치를 월드 좌표계로 역변환
        Matrix inverseProjView = (viewRow * projRow).Invert();

        // ViewFrustum 안에서 PickingRay의 방향 구하기
        Vector3 cursorWorldNear = Vector3::Transform(cursorNdcNear, inverseProjView);
        Vector3 cursorWorldFar = Vector3::Transform(cursorNdcFar, inverseProjView);
        Vector3 dir = cursorWorldFar - cursorWorldNear;
        dir.Normalize();

        // 광선을 만들고 충돌 감지
        SimpleMath::Ray curRay = SimpleMath::Ray(cursorWorldNear, dir);
        float dist = 0.0f;

        // Intersects함수는 ray와 object의 충돌을 체크
        m_selected = curRay.Intersects(m_mainBoundingSphere, dist);
```