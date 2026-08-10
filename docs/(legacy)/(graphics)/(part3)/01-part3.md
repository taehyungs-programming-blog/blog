---
layout: default
title: "01. 1인칭 시점"
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

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/1/1)

* (목표) FPS게임에서 제공해 주는 마우스를 이용한 시점 조절, WSAD를 이용한 움직임을 구현해보자.

```cpp
#pragma once

#include <directxtk/SimpleMath.h>

namespace hlab {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class Camera {
  public:
    Matrix GetViewRow();
    Matrix GetProjRow();
    Vector3 GetEyePos();

    void UpdateMouse(float mouseNdcX, float mouseNdcY);
    // ** 결국 구현해야하는 부분은 여기겠지? **
    void MoveForward(float dt);
    void MoveRight(float dt);
    void SetAspectRatio(float aspect);

  private:
    // 1인칭 시점은 FPS 게임을 떠올리시면 됩니다.
    // 가상 세계에 1인칭 시점 게임 캐릭터가 서있는 상황입니다.
    // 그 캐릭터의 눈의 위치에 카메라가 있습니다.
    // 그 캐릭터의 정면 방향이 카메라가 보는 방향입니다.

    // m_position : 월드 좌표계에서 카메라의 위치
    // m_viewDir : 카메라가 보는 방향, 걸어가는 방향
    // m_upDir : 위쪽 방향, 중력의 반대방향이 기본
    // m_rightDir : 오른쪽 방향, eyeDir과 upDir로부터 계산

    Vector3 m_position = Vector3(0.0f, 0.15f, 0.0f); // 0.15f는 눈높이 정도
    Vector3 m_viewDir = Vector3(0.0f, 0.0f, 1.0f);
    Vector3 m_upDir = Vector3(0.0f, 1.0f, 0.0f); // 이번 예제에서는 고정
    Vector3 m_rightDir = Vector3(1.0f, 0.0f, 0.0f);

    // roll, pitch, yaw
    // https://en.wikipedia.org/wiki/Aircraft_principal_axes
    float m_roll = 0.0f;
    float m_yaw = 0.0f;

    float m_speed = 1.0f; // 움직이는 속도

    // 프로젝션 옵션도 카메라 클래스로 이동
    float m_projFovAngleY = 70.0f;
    float m_nearZ = 0.01f;
    float m_farZ = 100.0f;
    float m_aspect = 16.0f / 9.0f;
    bool m_usePerspectiveProjection = true;
};
} // namespace hlab
```

```cpp
#include "Camera.h"

namespace hlab {

using namespace std;
using namespace DirectX;

Matrix Camera::GetViewRow() {
    // 렌더링에 사용할 View 행렬을 만들어주는 부분
    // 이번 예제에서는 upDir이 Y로 고정되었다고 가정합니다.
    // 시점 변환은 가상 세계가 통째로 반대로 움직이는 것과 동일
    // m_roll이 고개를 숙이는 회전이라서 -가 두번 붙어서 생략

    // TODO:
    return Matrix::CreateTranslation(-m_position);
}

Vector3 Camera::GetEyePos() { return m_position; }

void Camera::UpdateMouse(float mouseNdcX, float mouseNdcY) {
    // 얼마나 회전할지 계산
    // https://en.wikipedia.org/wiki/Aircraft_principal_axes
    m_yaw = mouseNdcX * DirectX::XM_2PI;     // 좌우 360도
    m_roll = mouseNdcY * DirectX::XM_PIDIV2; // 위 아래 90도

    // 이동할 때 기준이 되는 정면/오른쪽 방향 계산

    //TODO:
    //m_viewDir = ...;  // m_yaw만큼 회전
    //m_rightDir = ...; // Cross product 사용
}

void Camera::MoveForward(float dt) {
    // 이동후의_위치 = 현재_위치 + 이동방향 * 속도 * 시간차이;
    m_position += m_viewDir * m_speed * dt;
}

void Camera::MoveRight(float dt) {
    // 이동후의_위치 = 현재_위치 + 이동방향 * 속도 * 시간차이;
    m_position += m_rightDir * m_speed * dt;
}

void Camera::SetAspectRatio(float aspect) { m_aspect = aspect; }

Matrix Camera::GetProjRow() {
    return m_usePerspectiveProjection
               ? XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY),
                                          m_aspect, m_nearZ, m_farZ)
               : XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f,
                                                 1.0f, m_nearZ, m_farZ);
}
} // namespace hlab
```