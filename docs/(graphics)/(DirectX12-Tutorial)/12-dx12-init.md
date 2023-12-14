---
layout: default
title: "12. Scene"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/12)

## Scene

* Unity에도 아래와 같이 Scene이 등장하는데 간단하게 설명하면 GameObject를 관리하는 Manager의 개념이다. 
* 왜 이렇게 관리하느냐? Scene의 전환이 필요할때가 있고 등등등.. 뭐가 됐든 미래를 위해서 GameObject를 별도로 관리하는 클래스 자체가 필요하다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-11-1.png"/>
</p>

* 하고싶은것은 이것이다.

```cpp
void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

    // SceneManager에서 Scene을 로드하고
	GET_SINGLE(SceneManager)->LoadScene(L"TestScene");
}
```

```cpp
void SceneManager::LoadScene(wstring sceneName)
{
	// TODO : sceneName으로 로드하는 부분은 아직 구현안됨.

	_activeScene = LoadTestScene();

	_activeScene->Awake();
	_activeScene->Start();
}
```

```cpp
shared_ptr<Scene> SceneManager::LoadTestScene()
{
    // 아래처럼 Scene내에서 사용될 object를 정의한다.
	shared_ptr<Scene> scene = make_shared<Scene>();

	// TestObject
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

	gameObject->Init(); // Transform

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

	return scene;
}
```

---

## SceneManager Class

* 크게 어려울게 없어서 별도의 설명은 생략

```cpp
#pragma once

class Scene;

class SceneManager
{
	DECLARE_SINGLE(SceneManager);

public:
	void Update();
	void LoadScene(wstring sceneName);

public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }

private:
	shared_ptr<Scene> LoadTestScene();

private:
	shared_ptr<Scene> _activeScene;
};
```

```cpp
void SceneManager::Update()
{
	if (_activeScene == nullptr)
		return;

	_activeScene->Update();
/*
    void Scene::Update()
    {
        // 이 for문에서 주목할 것은 const shared_ptr<GameObject>&
        // 참조형으로 호출시 reference count가 올라가지 않아 성능에 좋음
        // 단, 함수내에서 많은 일을한다면 절대 이렇게 호출하면 안됨.
        for (const shared_ptr<GameObject>& gameObject : _gameObjects)
        {
            gameObject->Update();
        }
    }
*/
	_activeScene->LateUpdate();
}

void SceneManager::LoadScene(wstring sceneName)
{
	_activeScene = LoadTestScene();

	_activeScene->Awake();
	_activeScene->Start();
}
```

## Scene Class

```cpp
class Scene
{
public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();

	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

private:
	vector<shared_ptr<GameObject>> _gameObjects;
};
```