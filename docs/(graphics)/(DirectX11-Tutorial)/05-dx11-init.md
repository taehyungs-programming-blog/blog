---
layout: default
title: "05. Model"
parent: "(DirectX11 2D Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

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
#pragma once

struct ModelBone;
struct ModelMesh;

class Model : public enable_shared_from_this<Model>
{
public:
	Model();
	~Model();

public:
	// ReadXXX을 통해 읽는다
	void ReadMaterial(wstring filename);
	void ReadModel(wstring filename);

	uint32 GetMaterialCount() { return static_cast<uint32>(_materials.size()); }
	vector<shared_ptr<Material>>& GetMaterials() { return _materials; }
	shared_ptr<Material> GetMaterialByIndex(uint32 index) { return _materials[index]; }
	shared_ptr<Material> GetMaterialByName(const wstring& name);

	uint32 GetMeshCount() { return static_cast<uint32>(_meshes.size()); }
	vector<shared_ptr<ModelMesh>>& GetMeshes() { return _meshes; }
	shared_ptr<ModelMesh> GetMeshByIndex(uint32 index) { return _meshes[index]; }
	shared_ptr<ModelMesh> GetMeshByName(const wstring& name);

	uint32 GetBoneCount() { return static_cast<uint32>(_bones.size()); }
	vector<shared_ptr<ModelBone>>& GetBones() { return _bones; }
	shared_ptr<ModelBone> GetBoneByIndex(uint32 index) { return (index < 0 || index >= _bones.size() ? nullptr : _bones[index]); }
	shared_ptr<ModelBone> GetBoneByName(const wstring& name);

private:
	void BindCacheInfo();

private:
	wstring _modelPath = L"../Resources/Models/";
	wstring _texturePath = L"../Resources/Textures/";

private:
	shared_ptr<ModelBone> _root;
	vector<shared_ptr<Material>> _materials;
	vector<shared_ptr<ModelBone>> _bones;
	vector<shared_ptr<ModelMesh>> _meshes;
};
```

```cpp
void ModelRenderer::Update()
{
	if (_model == nullptr)
		return;

	// Bones
	BoneDesc boneDesc;

	const uint32 boneCount = _model->GetBoneCount();
	for (uint32 i = 0; i < boneCount; i++)
	{
		shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
		boneDesc.transforms[i] = bone->transform;
	}
	RENDER->PushBoneData(boneDesc);

	// Transform
	auto world = GetTransform()->GetWorldMatrix();
	RENDER->PushTransformData(TransformDesc{ world });

	const auto& meshes = _model->GetMeshes();
	for (auto& mesh : meshes)
	{
		if (mesh->material)
			mesh->material->Update();

		// BoneIndex
		_shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		uint32 stride = mesh->vertexBuffer->GetStride();
		uint32 offset = mesh->vertexBuffer->GetOffset();

		DC->IASetVertexBuffers(0, 1, mesh->vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
		DC->IASetIndexBuffer(mesh->indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

		_shader->DrawIndexed(0, _pass, mesh->indexBuffer->GetCount(), 0, 0);
	}
}
```

* 핵심은 이 ModelRenderer를 GameObject에 넣고

```cpp
class GameObject : public enable_shared_from_this<GameObject>
{
	// ...

	shared_ptr<Component> GetFixedComponent(ComponentType type);
	shared_ptr<Transform> GetTransform();
	shared_ptr<Camera> GetCamera();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<ModelRenderer> GetModelRenderer();
```

* GameObject Update시에 같이 Update가 된다.

---

## ImGui

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