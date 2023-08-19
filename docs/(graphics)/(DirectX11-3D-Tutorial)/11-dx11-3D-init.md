---
layout: default
title: "11. Collider + Picking"
parent: "(DirectX11 3D Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/10)

## Picking 사전에 알아야 할 사항

* 800x600 화면이 있다고 가정하면 이 화면을 -1~1 x -1~1 범위로 변경해야한다.
* 추가로 단순 x,y좌표만 알아선 안되고 깊이값 z좌표까지 알아야한다.
    * 왜냐면 앞쪽에 있는 물체를 선택됐다고 판별해야하기 때문.
* 이런 작업을 처리하기 위해서 ViewPort Class를 별도로 만들어준다

```cpp
Vec3 Viewport::Project(const Vec3& pos, const Matrix& W, const Matrix& V, const Matrix& P)
{
    // world 좌표계에서 projection된 좌표계로
	Matrix wvp = W * V * P;
	Vec3 p = Vec3::Transform(pos, wvp);

    // 아래식은 projection to pos로 받아들이자
	p.x = (p.x + 1.0f) * (_vp.Width / 2) + _vp.TopLeftX;
	p.y = (-p.y + 1.0f) * (_vp.Height / 2) + _vp.TopLeftY;
	p.z = p.z * (_vp.MaxDepth - _vp.MinDepth) + _vp.MinDepth;

	return p;
}

Vec3 Viewport::Unproject(const Vec3& pos, const Matrix& W, const Matrix& V, const Matrix& P)
{
    // projection 좌표계에서 world된 좌표계로
	Vec3 p = pos;

	p.x = 2.f * (p.x - _vp.TopLeftX) / _vp.Width - 1.f;
	p.y = -2.f * (p.y - _vp.TopLeftY) / _vp.Height + 1.f;
	p.z = ((p.z - _vp.MinDepth) / (_vp.MaxDepth - _vp.MinDepth));

	Matrix wvp = W * V * P;
	Matrix wvpInv = wvp.Invert();

	p = Vec3::Transform(p, wvpInv);
	return p;
}
```

* Tip) Viewport 변환행렬

```
M = 
W/2      0       0         0
0        -W/2    0         0
0        0       MaxD-MinD 0
W/2+Left W/2+Top MinD      1
```

---

## Sphere Collider

* Mesh에 직접 Collider를 붙이면 연산이 많아지니 Sphere를 하나 만들어두고 거기에 Collider를 붙인다.
* 자세한 설명 전 Picking하는 부분을 먼저 보자

```cpp
std::shared_ptr<class GameObject> Scene::Pick(int32 screenX, int32 screenY)
{
	shared_ptr<Camera> camera = GetCamera()->GetCamera();

	float width = GRAPHICS->GetViewport().GetWidth();
	float height = GRAPHICS->GetViewport().GetHeight();

	Matrix projectionMatrix = camera->GetProjectionMatrix();

	// Viewport의 좌표로 변환한다
	float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
	float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);

	Matrix viewMatrix = camera->GetViewMatrix();
	Matrix viewMatrixInv = viewMatrix.Invert();

	const auto& gameObjects = GetObjects();

	float minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;

	// 현재 그려지고 있는 gameObject를 순회
	for (auto& gameObject : gameObjects)
	{
		// Collider가 붙지 않았다면 continue
		if (gameObject->GetCollider() == nullptr)
			continue;

		// 0.0f, 0.0f, 0.0f는 ViewPort의 카메라 좌표를 의미한다
		Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

		// WorldSpace에서의 Ray 정의
		Vec3 worldRayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
		Vec3 worldRayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
		worldRayDir.Normalize();

		// WorldSpace에서 연산
		Ray ray = Ray(worldRayOrigin, worldRayDir);

		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects(ray, OUT distance) == false)
			continue;

		if (distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	// ...

	return picked;
}
```

* 이제 저 코드에서 BlackBox인 부분은 `GetCollider()->Intersects`

```cpp
class SphereCollider : public BaseCollider
{
public:
	SphereCollider();
	virtual ~SphereCollider();

	virtual void Update() override;
	virtual bool Intersects(Ray& ray, OUT float& distance) override;
	virtual bool Intersects(shared_ptr<BaseCollider>& other) override;

	void SetRadius(float radius) { _radius = radius; }
	BoundingSphere& GetBoundingSphere() { return _boundingSphere; }

private:
	float _radius = 1.f;
	// 사실 BoundingSphere는 DirectX lib를 그냥 땡겨쓴 것이다 ㅎㅎ.
	BoundingSphere _boundingSphere;
};
```

---

## Box Collider

* AABB, OBB Box Collider 개념이 나온다.
* 아래 그림을 보면 이해가 되는데

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-11-1.png"/>
</p>

* 비스듬히 있는 물체의 Collider설정을 그냥 Box로 해버리는게 정말 Collider로 의미가 있나?
* 틀어진 만큼 Collider의 축을 보정해줘야 하는게 아닌가? -> **OBB Collider**

```cpp
class OBBBoxCollider : public BaseCollider
{
public:
	OBBBoxCollider();
	virtual ~OBBBoxCollider();

	virtual void Update() override;
	virtual bool Intersects(Ray& ray, OUT float& distance) override;
	virtual bool Intersects(shared_ptr<BaseCollider>& other) override;

	BoundingOrientedBox& GetBoundingBox() { return _boundingBox; }

private:	
	BoundingOrientedBox _boundingBox;
};
```

```cpp
class AABBBoxCollider : public BaseCollider
{
public:
	AABBBoxCollider();
	virtual ~AABBBoxCollider();

	virtual void Update() override;
	virtual bool Intersects(Ray& ray, OUT float& distance) override;

	// 다른 Collider와 충돌의 의미
	virtual bool Intersects(shared_ptr<BaseCollider>& other) override;

	BoundingBox& GetBoundingBox() { return _boundingBox; }

private:
	BoundingBox _boundingBox;
};
```

* `BoundingOrientedBox`냐 `BoundingBox`냐 차이뿐임

---

