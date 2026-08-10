---
layout: default
title: "35. Mesh"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 4
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/taehyung77/DirextX-Example/tree/28)

## 준비

* FBX라는 포멧의 파일을 로드 후 출력하는 실습을 한다.
* 준비물 : [FBX SDK 🌍](https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-2-1)
    * Lib에 FBX복붙

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-35-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-35-2.png"/>
</p>

* [Free3D 🌎](https://free3d.com/ko/3d-models/fbx)에서 필요한 FBX를 다운받자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-35-3.png"/>
</p>

* 결과를 먼저보자면

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-35-4.png"/>
</p>

---

## 구현

```cpp
// Engine.Pch
// 사용준비를 하자

#ifdef _DEBUG
#pragma comment(lib, "FBX\\debug\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\debug\\libxml2-md.lib")
#pragma comment(lib, "FBX\\debug\\zlib-md.lib")
#else
#pragma comment(lib, "FBX\\release\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\release\libxml2-md.lib")
#pragma comment(lib, "FBX\\release\\zlib-md.lib")
#endif
```

```cpp
// wstring to string
// string to wstring


wstring s2ws(const string& s)
{
	int32 len;
	int32 slength = static_cast<int32>(s.length()) + 1;
	len = ::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	wstring ret(buf);
	delete[] buf;
	return ret;
}

string ws2s(const wstring& s)
{
	int32 len;
	int32 slength = static_cast<int32>(s.length()) + 1;
	len = ::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	string r(len, '\0');
	::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}
```

```cpp
// FBXLoader는 외부에서 복붙한 소스이기에 하나하나 분석은
// 해도되고 안해도 되고 ㅎ

// ...

class FBXLoader
{
public:
	FBXLoader();
	~FBXLoader();

    // ...
```

```cpp
#pragma region FBX
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Dragon.fbx");

		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetName(L"Dragon");
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 300.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			scene->AddGameObject(gameObject);
		}
	}
#pragma endregion
```