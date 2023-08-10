---
layout: default
title: "05. Model"
parent: "(DirectX11 Tutorial)"
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

---

