---
layout: default
title: "13. UI"
parent: "(DirectX11 3D Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/11)

## UI용 Camera를 만들어보자.

```cpp
void OrthographicDemo::Init()
{
	_shader = make_shared<Shader>(L"23. RenderDemo.fx");

    // 아래와 같이 카메라를 두 개 붙인다.

	// Camera
	{
		auto camera = make_shared<GameObject>();
		camera->GetOrAddTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
		camera->AddComponent(make_shared<Camera>());
		//camera->AddComponent(make_shared<CameraScript>());
		camera->GetCamera()->SetCullingMaskLayerOnOff(Layer_UI, true);
		CUR_SCENE->Add(camera);
	}

	// UI_Camera
	{
		auto camera = make_shared<GameObject>();
		camera->GetOrAddTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
		camera->AddComponent(make_shared<Camera>());
		camera->GetCamera()->SetProjectionType(ProjectionType::Orthographic);
		camera->GetCamera()->SetNear(1.f);
		camera->GetCamera()->SetFar(100.f);
		camera->AddComponent(make_shared<CameraScript>());

		camera->GetCamera()->SetCullingMaskAll();
		camera->GetCamera()->SetCullingMaskLayerOnOff(Layer_UI, false);
		CUR_SCENE->Add(camera);
	}

    // ...
```

* 랜더는 어떻게 할까?

```cpp
// Scene이 Render될때 camera를 Render하게 된다.
void Scene::Render()
{
	for (auto& camera : _cameras)
	{
		camera->GetCamera()->SortGameObject();
		camera->GetCamera()->Render_Forward();
	}
}
```

---

## Button 실습

```cpp
// 아래와 같이 버튼을 만들고 대충 느낌은 온다
{
    auto obj = make_shared<GameObject>();
    obj->AddComponent(make_shared<Button>());

    obj->GetButton()->Create(Vec2(100, 100), Vec2(100, 100), RESOURCES->Get<Material>(L"Veigar"));

    obj->GetButton()->AddOnClickedEvent([obj]() { CUR_SCENE->Remove(obj); });

    CUR_SCENE->Add(obj);
}
```

```cpp
void Scene::PickUI()
{
	if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON) == false)
		return;

	if (GetUICamera() == nullptr)
		return;

	POINT screenPt = INPUT->GetMousePos();

	shared_ptr<Camera> camera = GetUICamera()->GetCamera();

	const auto gameObjects = GetObjects();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetButton() == nullptr)
			continue;

		if (gameObject->GetButton()->Picked(screenPt))
			gameObject->GetButton()->InvokeOnClicked();
	}
}
```

```cpp
void Scene::Update()
{
	unordered_set<shared_ptr<GameObject>> objects = _objects;

	for (shared_ptr<GameObject> object : objects)
	{
		object->Update();
	}

	PickUI();
}
```

---