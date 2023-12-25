---
layout: default
title: "23. Frustum Culling"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/18)

## Frustum Culling

* 지금까지는 Mesh를 만들기만 하면 무조건 그리고 있다. 지금부터 **Frustum Culling**을 통해 그릴놈 안그릴놈 구분해주는 기능이라 생각하면 되겠다.

---

* 그럼 어떤 기준에서 Frustum Culling을 진행해야 할까?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-22-1.png"/>
</p>

* 위 그림과 같이 카메라에 찍힐 Mesh를 지정후 각 면을 기준으로 **내부에 속해있는지 외부에 있는지** 판별하면 된다.

* **내부에 속해있는지 외부에 있는지** 판별을 위해 느낌적인 느낌으로 내적(수직선을 긋는다)을 하면 될꺼 같은데 ... 자세한 수식의 정의를 보자

---

* 처음으로 3차원 평면의 식을 아래와 같이 정의해보자. `ax + by + cz + d = 0`
* 3차원 평면의 Normal Vector(평면에 수직인 벡터)는 N(a, b, c)가 되며, d의 경우 원점에서 평면까지의 거리가 된다.(이건 수학적인 부분이기에 받아들일 것)
	* 증명을 해보자면 한 평면위의 두 점 `A(x1, y1, z1), B(x2, y2, z2)`과
	* Normal Vector N을 내적시 0이 되면 `N(a, b, c)`가 Normal Vector임이 증명이 된다.
	* `AB = (x2 - x1, y2 - y1, z2 - z1), N(a, b, c)`를 내적해보자. (크게 중요하진 않기에 꼭 이해가 되지 않아도 됨.)

```
AB * N = a(x2 - x1) + b(y2 - y1) + c(z2 - z1)
AB * N = (ax2+by2+cz2) - (ax1+by1+cz1)
// ax2+by2+cz2+d = 0
// ax1+by1+cz1+d = 0
// 이기에
// ax2+by2+cz2 = -d
// ax1+by1+cz1 = -d
AB * N = 0      // 증명 끝!
```

* 이걸로 뭘 구할수 있을까?

```
    \
     A1(x, y, z)
      \
     / \
  d /   \
   /     \
 O        \
           \
```

* `OA1 * N` (벡터 OA1과 Normal Vector N의 내적)은 `d`(평면과 O과의 거리)가 된다. 그럼 평면위의 점이 아니라 좀 더 가까운 점 `A2`의 내적은?

```
                  \
  A2(x2, y2 z2)   A1(x, y, z)   <- 평면위의 점이 아니라 다른 점 A2
                    \
                     \
                      \
                       \
     O                  \
                         \
```

* `(x2)x + (y2)y + (z2)z + d < 0`이 되게 된다. 만약 평면보다 멀다면 `(x2)x + (y2)y + (z2)z + d > 0`

---

* **(결론)** 이제 카메라뷰의 각 평면(6개)의 Normal Vector를 구한 후 내적해서 0보다 큰지 작은지 구하면 카메라에 찍히는 영역내에 있는지 외부에 있는지 알 수 있다.

* 구현은 어떻게 할까? 일단 우리가 알고있는 카메라의 좌표는 Projection 좌표에서 x, y가 `-1 ~ 1`, depth가 `0 ~ 1`사이 임을 안다. Projection을 알기에 View좌표계로 변환 후 World좌표계로 다시 변환하면 World좌표계에서 카메라의 찍히는 영역을 구할수 있게 되고 카메라에 찍히는 영역내에 있는 Mesh만 그려주게 변경하면 된다.

```cpp
#pragma once

enum PLANE_TYPE : uint8
{
    // 카메라로 찍히는 여섯 평면에 대한 내적을 구해야 한다.
	PLANE_FRONT,
	PLANE_BACK,
	PLANE_UP,
	PLANE_DOWN,
	PLANE_LEFT,
	PLANE_RIGHT,

	PLANE_END
};

class Frustum
{
public:
	void FinalUpdate();

    // 그려할 평면 내부에 있는지 확인
	bool ContainsSphere(const Vec3& pos, float radius);

private:
	array<Vec4, PLANE_END> _planes;
};
```

```cpp
void Frustum::FinalUpdate()
{
	// Projection -> View -> World 을 해야하기에
	// View Inverse Matrix, Projection Inverse Matrix가 필요하다
	Matrix matViewInv = Camera::S_MatView.Invert();
	Matrix matProjectionInv = Camera::S_MatProjection.Invert();
	Matrix matInv = matProjectionInv * matViewInv;

	vector<Vec3> worldPos =
	{
		::XMVector3TransformCoord(Vec3(-1.f, 1.f, 0.f), matInv),		// 0
		::XMVector3TransformCoord(Vec3(1.f, 1.f, 0.f), matInv),			// 1
		::XMVector3TransformCoord(Vec3(1.f, -1.f, 0.f), matInv),		// 2
		::XMVector3TransformCoord(Vec3(-1.f, -1.f, 0.f), matInv),		// 3
		::XMVector3TransformCoord(Vec3(-1.f, 1.f, 1.f), matInv),		// 4
		::XMVector3TransformCoord(Vec3(1.f, 1.f, 1.f), matInv),			// 5
		::XMVector3TransformCoord(Vec3(1.f, -1.f, 1.f), matInv),		// 6
		::XMVector3TransformCoord(Vec3(-1.f, -1.f, 1.f), matInv)		// 7

		/*
        // 시계방향으로 좌표를 넣음을 주목
		      (4)------------(5)
	  	     /              /|
            /		       / |
		 (0)------------ (1) |
		   |              |  /(6)
		   |              | /
		   |              |/
          (3)------------(2)

            ** --> 오른쪽이  x **

                ^
             ** | 위가 y **

            ** 깊이가 z **

		*/
	};

    // XMPlaneFromPoints - 점 세개를 넣고 평면을 만들어 달라
	_planes[PLANE_FRONT] = ::XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2]);  // 시계 방향
	_planes[PLANE_BACK] = ::XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5]);   // 반시계 방향
	_planes[PLANE_UP] = ::XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1]);     // 시계 방향(CW)
	_planes[PLANE_DOWN] = ::XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6]);   // 반시계 방향(CCW)
	_planes[PLANE_LEFT] = ::XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7]);   // 시계 방향
	_planes[PLANE_RIGHT] = ::XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1]);  // 반시계 방향(CCW)
}

bool Frustum::ContainsSphere(const Vec3& pos, float radius)
{
	for (const Vec4& plane : _planes)
	{
		// n = (a, b, c)
		Vec3 normal = Vec3(plane.x, plane.y, plane.z);

		// ax + by + cz + d > radius
		if (normal.Dot(pos) + plane.w > radius)
			return false;
	}

	return true;
}
```

```cpp
void Camera::Render()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

	// TODO : Layer 구분
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

        // Frustum을 적용하지 말아야 할 object도 존재한다(skybox)
		if (gameObject->GetCheckFrustum())
		{
            // 랜더링 할지 말지 결정
			if (_frustum.ContainsSphere(
				gameObject->GetTransform()->GetWorldPosition(),
				gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
			{
				continue;
			}
		}

		gameObject->GetMeshRenderer()->Render();
	}
}
```