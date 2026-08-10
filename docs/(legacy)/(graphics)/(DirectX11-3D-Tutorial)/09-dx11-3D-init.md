---
layout: default
title: "09. Scene 구조"
parent: "(DirectX11 3D Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/8)

```cpp
// SceneManager를 통해 현재 Scene을 관리
class SceneManager
{
	DECLARE_SINGLE(SceneManager);

public:
	void Update();

	template<typename T>
	void ChangeScene(shared_ptr<T> scene)
	{
		_currentScene = scene;
		scene->Start();
	}

	shared_ptr<Scene> GetCurrentScene() { return _currentScene; }

private:
	shared_ptr<Scene> _currentScene = make_shared<Scene>();
};
```

```cpp
class Scene
{
public:
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();

	virtual void Add(shared_ptr<GameObject> object);
	virtual void Remove(shared_ptr<GameObject> object);

private:
	unordered_set<shared_ptr<GameObject>> _objects;
	// Cache Camera
	unordered_set<shared_ptr<GameObject>> _cameras;
	// Cache Light
	unordered_set<shared_ptr<GameObject>> _lights;
};
```