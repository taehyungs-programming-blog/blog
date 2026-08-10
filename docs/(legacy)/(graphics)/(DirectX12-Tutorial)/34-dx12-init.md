---
layout: default
title: "34. Picking"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 4
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/taehyung77/DirextX-Example/tree/27)

## 이론

* **Picking** - DirectX로 만들어진 GameObject에 클릭처리를 받게 하고싶다
    * 사실 DirectX Present되는 화면은 그냥 이미지일뿐인데 어떻게 Input이벤트를 받을까?
    * 힌트) 화면에 일종의 레이저를 쏜다.(**Ray Casting**) 레이저를 맞으면 Hit 처리!

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-34-1.jpg"/>
</p>

* 화면에 그려지는 절차를 다시 생각해 보면
    * Object Space(Local Space) - 물체 자체적 위치
    * World Space - 월드에서의 위치
    * Eye Space(Camera Space) - 카메라가 찍는 위치
    * Clip Space(Projection Space) - 화면에 어떻게 보일지 결정
    * Screen Space(View Space) - 화면에 보여질 비률로 변경

* 자! 이제 마우스 클릭을 받을시 거꾸로 올라가면 된다.(말은 쉽다)

---

### View to Projection Space

```
-----------------
|               |
|               |
|               | height (h)
|               |
|               |
-----------------
     width (w)
```

```
x = (2*x)/w - 1
y = -(2*y)/h + 1
```

---

### Projection to Camera Space

```
x = x * r*tan(a/2)
y = y * tan(a/2)
```

* Projection Matrix가

```
1/(r*tan(a/2))  1           1   1
1               1/tan(a/2)  1   1
1               1           A   1
1               1           B   0
```

* 이기에 아래와 같이 표현이 가능하다.

```
x = x * r*tan(a/2) = x / P(0,0)
y = y * tan(a/2) = y / P(1,1)
```

```cpp
shared_ptr<GameObject> SceneManager::Pick(int32 screenX, int32 screenY)
{
	shared_ptr<Camera> camera = GetActiveScene()->GetMainCamera();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	Matrix projectionMatrix = camera->GetProjectionMatrix();

	// ViewSpace에서 Picking 진행
	float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
	float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);
```

---

### Camera to World Space

* 여기가 진짜 충돌판정이다.
    * 사실 아래의 증명은 그리 의미없음 **DirectX에 함수**가 있음
    * 저렇게 구한다고만 알아두자.

```
// 원을 다음과 같이 표현가능

| p - c | = d
// p - 원의 임의의 점(현재는 원의 표면에 맞을 경우를 가정)
// c - 원점
// d - 반지름

r = q + tu
// r - ray(직선을 의미)
// q - 임의의 점
// u - 방향성을 의미 상수 t만큼 방향성을 갖는 빛을 의미한다.
```

```
// 원의 임의의 점 p를 r로 대체해 보면

| q + tu - c | = d
d^2 = (q - c + tu) * (q - c + tu)
    = (m*m) + (t*2m*u) + (t^2*u*u)

// 이렇게 정리가능
(u*u)*t^2 + (2*m*u)*t + (m*m-d^2) = 0

a = (u*u)
b = (2*m*u)
c = (m*m-d^2)
```

* 우린 `y = ax^2 + bx + c`일때 
    * 근이 2개, 1개 일때 ray가 접함을 알고
    * 근이 1개일때 ray가 접하지 않음을 안다.

```cpp
shared_ptr<GameObject> SceneManager::Pick(int32 screenX, int32 screenY)
{
	shared_ptr<Camera> camera = GetActiveScene()->GetMainCamera();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	Matrix projectionMatrix = camera->GetProjectionMatrix();

	// ViewSpace에서 Picking 진행
	float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
	float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);

	Matrix viewMatrix = camera->GetViewMatrix();
	Matrix viewMatrixInv = viewMatrix.Invert();

	auto& gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();

	float minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetCollider() == nullptr)
			continue;

		// ViewSpace에서의 Ray 정의
		Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
            // 카메라의 위치가 0, 0, 0이기에 
		Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);
            // ray의 방향성

		// WorldSpace에서의 Ray 정의
		rayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
		rayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
		rayDir.Normalize();

		// WorldSpace에서 연산
		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false)
			continue;

		if (distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	return picked;
}
```

---

## 그 외의 구현

```cpp
enum class ColliderType
{
	Sphere,
};

class BaseCollider : public Component
{
public:
	BaseCollider(ColliderType colliderType);
	virtual ~BaseCollider();

	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) = 0;

private:
	ColliderType _colliderType = {};
};
```

```cpp
bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingSphere.Intersects(rayOrigin,        // 광선의 시작점
                                        rayDir,         // 광선의 방향
                                        OUT distance);  // 충돌이 됐다면 distance가 들어옴
}
```