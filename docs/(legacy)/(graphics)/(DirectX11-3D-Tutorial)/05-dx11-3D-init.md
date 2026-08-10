---
layout: default
title: "05. assimp lib + 모델 띄우기"
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

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/6)

## Assimp

* 모델(.fbx)의 로드를 위해서 **Assimp** 라이브러리를 사용한다
    * 로드하려는 이유는 우리가 원하는 타입으로 변경하려함!
* [assimp lib 🌍](https://github.com/assimp/assimp)를 빌드해서 사용하면 된다.

```cpp
// 대략 아래와 같이 쓰고싶다
    // -> Converter를 만들어 보자
void AssimpTool::Init()
{
	{
		shared_ptr<Converter> converter = make_shared<Converter>();

		// FBX -> Memory
		converter->ReadAssetFile(L"Tank/Tank.fbx");

		// Memory -> CustomData (File)
		converter->ExportMaterialData(L"Tank/Tank");
		converter->ExportModelData(L"Tank/Tank");

		// CustomData (File) -> Memory
	}
    // ...
}
```

* Convert의 코드가 아주긴데 주목해야할 만한 부분만 정리하자면

```cpp
void Converter::ReadAssetFile(wstring file)
{
	wstring fileStr = _assetPath + file;

	auto p = std::filesystem::path(fileStr);
	assert(std::filesystem::exists(p));

    // 이렇게 로드하는데 아래 aiProcess관련옵션이 로드하면서 이렇게 처리해 주세요~ 이런옵션이다.
    // 이런 처리가 들어가기에 로딩속도가 오래걸리는 것이다.
	_scene = _importer->ReadFile(
		Utils::ToString(fileStr),
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	assert(_scene != nullptr);
}
```

```cpp
// Material 정보를 Export한다
void Converter::ExportMaterialData(wstring savePath)
{
	wstring finalPath = _texturePath + savePath + L".xml";
	ReadMaterialData();
	WriteMaterialData(finalPath);
}

// Mesh 정보를 Export한다
void Converter::ExportModelData(wstring savePath)
{
	wstring finalPath = _modelPath + savePath + L".mesh";
	ReadModelData(_scene->mRootNode, -1, -1);
	WriteModelFile(finalPath);
}
```

* 여기까지하면 **.xml**과 **.mesh**로 데이터를 추출했다
* 이제 이걸 어떻게 사용하냐!
	* 참고로 **FileUtils**를 통해 파일을 읽고, 쓰는법을 정리했는데 필요하다면 보자(간단해서 생략)

---

## 모델 띄우기

```cpp
void StaticMeshDemo::Init()
{
	RESOURCES->Init();
	_shader = make_shared<Shader>(L"15. ModelDemo.fx");

	// Camera
	_camera = make_shared<GameObject>();
	_camera->GetOrAddTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
	_camera->AddComponent(make_shared<Camera>());
	_camera->AddComponent(make_shared<CameraScript>());

	//CreateTower();
	CreateTank();

	RENDER->Init(_shader);
}

// ...

void StaticMeshDemo::CreateTower()
{
	// CustomData -> Memory
	shared_ptr<class Model> m1 = make_shared<Model>();
	m1->ReadModel(L"Tower/Tower");
	m1->ReadMaterial(L"Tower/Tower");

	_obj = make_shared<GameObject>();
	_obj->GetOrAddTransform()->SetPosition(Vec3(0, 0, 50));
	_obj->GetOrAddTransform()->SetScale(Vec3(1.0f));

	_obj->AddComponent(make_shared<ModelRenderer>(_shader));
	{
		_obj->GetModelRenderer()->SetModel(m1);
		//_obj->GetModelRenderer()->SetPass(1);
	}
}
```

---

## Tips) ImGui 써보기

* [ImGui Github 🌎](https://github.com/ocornut/imgui)에서 코드를 받자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-5-1.png"/>
</p>

* ImGui를 사용하기 위해 필요한 파일은 위 정도이고 필요에 따라 추가하면된다.
* 실제사용은 아래와 같이하면 된다.

```cpp
// ImGUI
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
```

```cpp
// 주의할 점은 키보드, 마우스 처리를 위해 아래를 넣어줘야한다는것!

LRESULT CALLBACK Game::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(handle, message, wParam, lParam))
		return true;

	// ...
```

* 우선 ImGuiManager를 사용해 렌더에 필요한 부분을 넣자

```cpp
void ImGuiManager::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(GAME->GetGameDesc().hWnd);
	ImGui_ImplDX11_Init(DEVICE.Get(), DC.Get());
}

void ImGuiManager::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::Render()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
```

* 이렇게하면 ImGuiDemo에서는 컨텐츠만 넣을 수 있다

```cpp
void ImGuiDemo::Test()
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}
```