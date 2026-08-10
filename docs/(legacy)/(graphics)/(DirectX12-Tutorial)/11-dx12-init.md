---
layout: default
title: "11. Component"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/11)

* 게임엔진은 Object -> Object를 쌓아서 만드는 구조이다
* 하나하나의 Object를 Component로 정의하고 필요한 Component를 넣는식으로 하나의 Object를 만든다(그말이 그말인가?)
    * 참고로 Unity의 경우 위 설명처럼 Component를 추가해서 Object를 만드는개념이고
    * Unreal의 경우 상속을 통해서 Object를 만드는 개념이다.
    * Unreal이 조금 더 보수적(자유도가 낮다)이다.
* 여기서 만드려는 것은 Component를 만들어 보려한다.

---

```
* GameObject아래 Component가 있는 형태이고 *

 GameObject --- Component
             |
             -- Component
             |
             -- Component

* Component는 현재 *
MeshRenderer, Transform, MonoBehavior 세 가지가 있음

* UML *
Component <--- MeshRenderer
          <--- Transform
		  <--- MonoBehavior
```

```cpp
// 사용되는 Object를 Component 기반으로 만들 예정
class Component
{
public:
	Component(COMPONENT_TYPE type);
	virtual ~Component();


그리고 이런 Component들을 Gameobject로 모아서 관리할 예정

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	virtual ~GameObject();

	void Init();

	void Awake();
	void Start();
	void Update();
	void LateUpdate();

	shared_ptr<Transform> GetTransform();

	void AddComponent(shared_ptr<Component> component);

private:
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
	vector<shared_ptr<MonoBehaviour>> _scripts;
};
```

```cpp
// 아직은 MeshRenderer 정도만 만들어보자

class MeshRenderer : public Component
{
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }

	virtual void Update() override { Render(); }

	void Render();

private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;
};
```

```cpp
shared_ptr<GameObject> gameObject = make_shared<GameObject>();

void Game::Init(const WindowInfo& info)
{
	// ...

	gameObject->Init(); // Transform

	// ...

	gameObject->AddComponent(meshRenderer);	

	GEngine->GetCmdQueue()->WaitSync();
}
```

```cpp
void Game::Update()
{
	GEngine->Update();

	GEngine->RenderBegin();

	gameObject->Update();

	GEngine->RenderEnd();
}

// 사실상 내용은 쉽다.
```