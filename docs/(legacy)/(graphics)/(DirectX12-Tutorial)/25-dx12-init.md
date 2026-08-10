---
layout: default
title: "25. Orthographic Projection(직교투영)"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/19)

## 이론

* 지금까지 사용하던 Projection(투영)은 원근투영, 멀리있으면 작게 가까이있으면 크게 나타내는 투영방법이다.
* 그럼 직교투영은 거리에 상관없이 일정하게 나타나게하는 투영법이다. 예로들자면 게임화면의 UI등이 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-25-1.png"/>
</p>

* Orthographic Matrix를 수식으로 표현해 보자면

```
      --------------
     /             /|
 (z)/             / |
   /             /  | (h)
  --------------    |
  |            |    |
  |            |  /
  |            | /   
  |            |/    
  -------------
       (w)
```

```
x = 2x/w
y = 2y/h
z = (z/(f-n))-(n/(f-n))
// z의 식이 복잡해 보이지만
// 대충 증명하면 z에 f를 넣으면 1나오고
// z에 n을 넣으면 0이 나온다.

// 이기에
```

```
2/w 0   0        0
0   2/h 0        0
0   0   1/(f-n)  0
0   0   -n/(f-n) 1

* f : far
* n : near
```

* 가 Orthographic Matrix가 된다. 
    * (그냥 받아들여도 무방하고, DirectX에서 위 Matrix를 지원하기에 앞으로도 볼 일이 크게 없음.)

---

## 구현

* 구현을 위해 필요한 작업
    * 카메라를 두 개로 나눠야 한다.(Projection, Orthographic)
    * 화면에 그려질때 두 개로 나눈 카메라를 하나로 합쳐 그린다.
* 대략이렇게 할 예정
	* layer를 만들고 gameobject는 자신의 layer를 갖는다
	* rendering할때 layer를 보고 그릴지 말지를 판단 혹은 ui로 그릴지 판단

```cpp
// 우선은 UI인지 아닌지 판별을 위해 Layer를 만들자

// SceneManager.h

enum
{
	// Layer를 만들고 각 카메라가 찍어야하는 Layer를 분리하자
	MAX_LAYER = 32
};

class SceneManager
{
	// ...

	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return _layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);

	// ...

	// Layer에 따라 어떤 역할을 하는지 이름으로 관리
	array<wstring, MAX_LAYER> _layerNames;
    // Layer에 따라 몇 번째 있는지 index로 관리
	map<wstring, uint8> _layerIndex;
};
```

```cpp
void SceneManager::SetLayerName(uint8 index, const wstring& name)
{
	// 기존 데이터 삭제
	const wstring& prevName = _layerNames[index];
	_layerIndex.erase(prevName);

	_layerNames[index] = name;
	_layerIndex[name] = index;
}

uint8 SceneManager::LayerNameToIndex(const wstring& name)
{
	auto findIt = _layerIndex.find(name);
	if (findIt == _layerIndex.end())
		return 0;

	return findIt->second;
}
```

* 각 GameObject는 자신의 LayerIndex를 갖고있어야 한다.

```cpp
class GameObject : public Object, public enable_shared_from_this<GameObject>
{
    // ...

	void SetLayerIndex(uint8 layer) { _layerIndex = layer; }
	uint8 GetLayerIndex() { return _layerIndex; }

    // ...

	bool _checkFrustum = true;
	uint8 _layerIndex = 0;
};
```

* 카메라의 경우 자신이 찍을 Object의 Layer를 알고있어야 한다.

```cpp
class Camera : public Component
{
public:
	// ...

	void SetCullingMaskLayerOnOff(uint8 layer, bool on)
	{
		if (on)
			_cullingMask |= (1 << layer);
		else
			_cullingMask &= ~(1 << layer);
	}
    // 특정 Layer만 찍을지 찍지말지 결정

	void SetCullingMaskAll() { SetCullingMask(UINT32_MAX); }
    // 아무것도 찍지 않겠다.
	void SetCullingMask(uint32 mask) { _cullingMask = mask; }
	bool IsCulled(uint8 layer) { return (_cullingMask & (1 << layer)) != 0; }
    // 찍을지 말지 물어본다.

private:
	PROJECTION_TYPE _type = PROJECTION_TYPE::PERSPECTIVE;

	// ...

	Frustum _frustum;
	uint32 _cullingMask = 0;

    // ...
```

```cpp
void Camera::Render()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

	// TODO : Layer 구분
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

        // 찍을 대상인지 아닌지 확인
		if (IsCulled(gameObject->GetLayerIndex()))
			continue;

		if (gameObject->GetCheckFrustum())
		{
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

```cpp
void Camera::FinalUpdate()
{
	_matView = GetTransform()->GetLocalToWorldMatrix().Invert();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	if (_type == PROJECTION_TYPE::PERSPECTIVE)
		_matProjection = ::XMMatrixPerspectiveFovLH(_fov, width / height, _near, _far);
	else
        // 직교투영 처리방법
		_matProjection = ::XMMatrixOrthographicLH(width * _scale, height * _scale, _near, _far);

	_frustum.FinalUpdate();
}
```

```cpp
shared_ptr<Scene> SceneManager::LoadTestScene()
{
#pragma region LayerMask
	SetLayerName(0, L"Default");
	SetLayerName(1, L"UI");
#pragma endregion

    // ...

    // 카메라 선언
#pragma region Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Main_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45도
		camera->AddComponent(make_shared<TestCameraScript>());
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
		scene->AddGameObject(camera);
	}	
#pragma endregion

#pragma region UI_Camera        // Orthographic Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll(); // 다 끄고
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음

		// scene안에 camera를 두 개 다 넣음을 기억.
		// 나중에 어디서 camera가 찍은 그림을 합치는지 헷갈림
		scene->AddGameObject(camera);
	}
#pragma endregion

    // ...

	// UI가 될 Object추가

#pragma region UI_Test
	{
		shared_ptr<GameObject> sphere = make_shared<GameObject>();
		sphere->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		sphere->AddComponent(make_shared<Transform>());
		sphere->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		sphere->GetTransform()->SetLocalPosition(Vec3(0, 0, 500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Forward");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Leather", L"..\\Resources\\Texture\\Leather.jpg");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		sphere->AddComponent(meshRenderer);
		scene->AddGameObject(sphere);
	}
#pragma endregion
```

```cpp
shared_ptr<Mesh> Resources::LoadRectangleMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Rectangle");
	if (findMesh)
		return findMesh;

	float w2 = 0.5f;
	float h2 = 0.5f;

	vector<Vertex> vec(4);

	// 앞면
	vec[0] = Vertex(Vec3(-w2, -h2, 0), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(-w2, +h2, 0), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(Vec3(+w2, +h2, 0), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(Vec3(+w2, -h2, 0), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(6);

	// 앞면
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Init(vec, idx);
	Add(L"Rectangle", mesh);

	return mesh;
}
```

```cpp
void Resources::CreateDefaultShader()
{
	// Skybox
	{
		ShaderInfo info =
		{
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::LESS_EQUAL
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"..\\Resources\\Shader\\skybox.fx", info);
		Add<Shader>(L"Skybox", shader);
	}

	// Forward (Forward) : Default 쉐이더에서 이름 변경
	{
		ShaderInfo info =
		{
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"..\\Resources\\Shader\\forward.fx", info);
		Add<Shader>(L"Forward", shader);
	}
}
```