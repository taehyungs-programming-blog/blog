---
layout: default
title: "16. Camera"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 앞에서 배운 좌표계 변환을 적용해 보자.
    * **Local -> World -> View -> Projection -> Screen**

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/13)

## 우선! matrix연산을 위해 수학과 관련된 라이브러리를 받아보자

 * [DirectXTK12](https://github.com/microsoft/DirectXTK)에서 `SimpleMath`의 h/cpp/inl을 가져온다.
    * 참고로 inl은 inline을 모두 담아둔 파일이다.
* 역시 vcpkg를 써도 될지도? `vcpkg install directxtk12:x64-windows`

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-15-1.png"/>
</p>

```cpp
// EnginePch.h
#include "SimpleMath.h"

// ...

using Vec2		= DirectX::SimpleMath::Vector2;
using Vec3		= DirectX::SimpleMath::Vector3;
using Vec4		= DirectX::SimpleMath::Vector4;
using Matrix	= DirectX::SimpleMath::Matrix;

// Vector4, Matrix 내부에 Vector연산, Matrix연산에 관한 부분이 모두 포함이 되어있다
    // 예를들어 Matrix.Forward(); -> 전방의 단위백터를 구할수 있음 등등 엄청 많다

// 추가로 DirectX::SimpleMath::Matrix는 오른손 좌표계를 사용한다.
// 앞서 계산한 식은 왼손좌표계 기준으로 계산했기에 아래서 조정할 예정, 
// 그냥 미리 알아두자 ㅎ

// ...
```

### (참고) **union?** 이란

```cpp
union
{
    struct
    {
        float _11, _12, _13, _14;
        float _21, _22, _23, _24;
        float _31, _32, _33, _34;
        float _41, _42, _43, _44;
    };
    float m[4][4];
};
```

* 변수는 하나만 생성되고 접근을 두 가지방법으로 가능
    * `_11`로 접근가능 
    * `m[0][0]`으로도 접근가능

---

## Transform 클래스

* 3D Object의 계층구조 개념이 나오는데

```
Parent Object
      |
       ----> Child Object
      |
       ----> Child Object
```

* 예를들어 Parent Object가 사람의 몸통이라하면 Child Object는 양팔정도라고 생각하자
* 그럼 몸통이 이동시 양팔도 동시에 움직여야한다.
* 이런 이동까지 반영햐여 Transform class를 구현해야 한다.

```cpp
class Transform : public Component
{
	// ...

private:
	// Parent 기준에서 SRT가 어떻게 되는지이지
        // Wrold기준 아님을 주의하자!
	Vec3 _localScale = { 1.f, 1.f, 1.f };   // Scale
	Vec3 _localRotation = {};               // Rotation
	Vec3 _localPosition = {};               // Translation

	// Parent 기준 Matrix
	Matrix _matLocal= {};

	// World 기준 Matrix
	Matrix _matWorld = {};

	weak_ptr<Transform> _parent;
};
```

```cpp
// 위치 정보를 받는 함수 필요

public:
	// Parent 기준
	const Vec3& GetLocalPosition() { return _localPosition; }
	const Vec3& GetLocalRotation() { return _localRotation; }
	const Vec3& GetLocalScale() { return _localScale; }

	const Matrix& GetLocalToWorldMatrix() { return _matWorld; }
	const Vec3& GetWorldPosition() { return _matWorld.Translation(); }

	Vec3 GetRight() { return _matWorld.Right(); }
	Vec3 GetUp() { return _matWorld.Up(); }
	Vec3 GetLook() { return _matWorld.Backward(); }

	void SetLocalPosition(const Vec3& position) { _localPosition = position; }
	void SetLocalRotation(const Vec3& rotation) { _localRotation = rotation; }
	void SetLocalScale(const Vec3& scale) { _localScale = scale; }

public:
	void SetParent(shared_ptr<Transform> parent) { _parent = parent; }
	weak_ptr<Transform> GetParent() { return _parent; }
```

```cpp
void Transform::FinalUpdate()
{
	// World Matrix를 만들어주는 과정이다.


	// (S) Scale
	Matrix matScale = Matrix::CreateScale(_localScale);

	// (R) Rotation
        // 나중에 쿼터니언으로 수정해 줘야함.
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);

	// (T) Translation
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	// Parent를 기준으로한 Local Matrix임을 잊지 말자
	_matLocal = matScale * matRotation * matTranslation;
	_matWorld = _matLocal;

	shared_ptr<Transform> parent = GetParent().lock();

	// Parent가 없다면 그냥 Local이 World Matrix가 된다
	if (parent != nullptr)
	{
		// 부모가 있다면 부모의 World Matrix을 곱해준다.
		_matWorld *= parent->GetLocalToWorldMatrix();
	}
}

void Transform::PushData()
{
	// World -> View -> Projection이 필요한데
	// View, Projection은 Camera Component에서 관리한다.
	// 따라서 Camera 클래스의 구현이 끝나고 다시 설명한다.

	Matrix matWVP = _matWorld * Camera::S_MatView * Camera::S_MatProjection;
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushData(&matWVP, sizeof(matWVP));
    // using Matrix	= DirectX::SimpleMath::Matrix; 인데
         // DirectX::SimpleMath::Matrix를 일단 레지스터에 넣는다고 받아들이자
         // 쓰는법은 아래서 나온다
}
```

* 이제 Transform 정보를 GPU레지스터에 넘겨야하는데 따라서 Constant Buffer할당시 할당되는 변수도 달라진다.

```cpp
// EnginePch.h

// ...

struct TransformParams
{
	Matrix matWVP;
};
```

```cpp
void Engine::Init(const WindowInfo& info)
{
	// ...

	// b0 Constant buffer에 TransformParams을 할당
	CreateConstantBuffer(CBV_REGISTER::b0, sizeof(TransformParams), 256);
	CreateConstantBuffer(CBV_REGISTER::b1, sizeof(MaterialParams), 256);
```

```cpp
// 쉐이더 역시 TRANSFORM_PARAMS로 받게 한다.
cbuffer TRANSFORM_PARAMS : register(b0)
{
    row_major matrix matWVP;
    // row_major : DirectX 기준에서는 행을 기준으로 데이터를 읽고 m[0][0] -> m[0][1] -> m[0][2] ...
        // 쉐이더는 열을 기준으로 데이터를 읽는데 m[0][0] -> m[1][0] -> m[2][0] ...
        // 행을 기준으로 데이터를 읽어달라는 명령
};

// ...

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.f), matWVP);
	// 좌표에 matMVP를 반영해 달라.

    output.color = input.color;
    output.uv = input.uv;

    return output;
}

// ...
```

---

* 이제 View, Projection이 남았다.

## Camera 클래스

```cpp
#pragma once
#include "Component.h"

enum class PROJECTION_TYPE
{
	PERSPECTIVE, 		// 원근 투영
	ORTHOGRAPHIC, 		// 직교 투영
};

class Camera : public Component
{
public:
	Camera();
	virtual ~Camera();

	virtual void FinalUpdate() override;
	void Render();

private:
	PROJECTION_TYPE _type = PROJECTION_TYPE::PERSPECTIVE;

	float _near = 1.f;
	float _far = 1000.f;
	float _fov = XM_PI / 4.f;		// 기본 45도(pi/4)
	float _scale = 1.f;

	Matrix _matView = {};
	Matrix _matProjection = {};

public:
	// static은 임시(TEMP)
	static Matrix S_MatView;
	static Matrix S_MatProjection;
};
```

```cpp
// ...

Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;

// ...

void Camera::FinalUpdate()
{
	// World Matrix를 역행렬취하면 View가 나온다.
        // 대략 이론적으로 설명하자면
            // _matView는 카메라의 Matrix고 카메라가 찍는 방향의 역행렬이 찍히는 방향이 된다.
            // 따라서 카메라의 Invert는 카메라가 찍는 Object가 된다.
	_matView = GetTransform()->GetLocalToWorldMatrix().Invert();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	// Projection 좌표계는 DirectX지원함수로 구한다.
	if (_type == PROJECTION_TYPE::PERSPECTIVE)
		_matProjection = ::XMMatrixPerspectiveFovLH(_fov, width / height, _near, _far);
	else
		// 직교투영, 원근법을 적용하지 않음.
		_matProjection = ::XMMatrixOrthographicLH(width * _scale, height * _scale, _near, _far);

	// 여기는 임시로 사용하는 부분이다.
	S_MatView = _matView;
	S_MatProjection = _matProjection;
}
```

```cpp
void Transform::PushData()
{
	Matrix matWVP = _matWorld * Camera::S_MatView * Camera::S_MatProjection;
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushData(&matWVP, sizeof(matWVP));
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

		gameObject->GetMeshRenderer()->Render();
	}
}
```

---

## 카메라를 움직여 보자

```cpp
#pragma once
#include "MonoBehaviour.h"

class TestCameraScript : public MonoBehaviour
{
public:
	TestCameraScript();
	virtual ~TestCameraScript();

	virtual void LateUpdate() override;

private:
	float		_speed = 100.f;
};
```

```cpp
void TestCameraScript::LateUpdate()
{
	Vec3 pos = GetTransform()->GetLocalPosition();

	if (INPUT->GetButton(KEY_TYPE::W))
		pos += GetTransform()->GetLook() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::S))
		pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::A))
		pos -= GetTransform()->GetRight() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::D))
		pos += GetTransform()->GetRight() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::Q))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x += DELTA_TIME * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::E))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x -= DELTA_TIME * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	GetTransform()->SetLocalPosition(pos);
}
```

* Camera와 Object를 별도로 만들어 줘야한다.

```cpp
void SceneManager::LoadScene(wstring sceneName)
{
	// TODO : 기존 Scene 정리
	// TODO : 파일에서 Scene 정보 로드

	_activeScene = LoadTestScene();

	_activeScene->Awake();
	_activeScene->Start();
}

shared_ptr<Scene> SceneManager::LoadTestScene()
{
	shared_ptr<Scene> scene = make_shared<Scene>();

#pragma region TestObject
	shared_ptr<GameObject> gameObject = make_shared<GameObject>();

	vector<Vertex> vec(4);
	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[0].uv = Vec2(0.f, 0.f);
	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[1].uv = Vec2(1.f, 0.f);
	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vec[2].uv = Vec2(1.f, 1.f);
	vec[3].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[3].uv = Vec2(0.f, 1.f);

	vector<uint32> indexVec;
	{
		indexVec.push_back(0);
		indexVec.push_back(1);
		indexVec.push_back(2);
	}
	{
		indexVec.push_back(0);
		indexVec.push_back(2);
		indexVec.push_back(3);
	}

	gameObject->AddComponent(make_shared<Transform>());
	shared_ptr<Transform> transform = gameObject->GetTransform();
	transform->SetLocalPosition(Vec3(0.f, 100.f, 200.f));
	transform->SetLocalScale(Vec3(100.f, 100.f, 1.f));

	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->Init(vec, indexVec);
		meshRenderer->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shared_ptr<Texture> texture = make_shared<Texture>();
		shader->Init(L"..\\Resources\\Shader\\default.hlsli");
		texture->Init(L"..\\Resources\\Texture\\veigar.jpg");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetFloat(0, 0.3f);
		material->SetFloat(1, 0.4f);
		material->SetFloat(2, 0.3f);
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);
	}
	gameObject->AddComponent(meshRenderer);
	scene->AddGameObject(gameObject);
#pragma endregion

#pragma region Camera
	shared_ptr<GameObject> camera = make_shared<GameObject>();
	camera->AddComponent(make_shared<Transform>());
	camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45도
	camera->AddComponent(make_shared<TestCameraScript>());
	camera->GetTransform()->SetLocalPosition(Vec3(0.f, 100.f, 0.f));
	scene->AddGameObject(camera);
#pragma endregion

	return scene;
}
```