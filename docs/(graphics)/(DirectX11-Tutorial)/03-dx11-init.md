---
layout: default
title: "03. Engine구조 변경"
parent: "(DirectX11 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 다음 진행 코드에만 집중할수 있게 코드 구조를 바꾸려한다.
	* **Client** - 실습에 필요한 코드
	* **Engine** - DirectX관련 코드(실습과는 무방)
* 이렇게 수정하려하기에 ... 이번장은 코드가 어떻게 수정되었고 왜 이렇게 수정했는지에 대한 설명이다.

---

* [Get Code 🌍]()

## 0. Engine

* 엔진을 사용하는데 호출되는 클래스를 정의한다
* **Resource**와 **Execute**
	* Resource는 또 대표적인 Resource인 Mesh와 Texture로 상속된다.

```cpp
enum class ResourceType : uint8
{
	None = -1,

	// 아래와 같이 리소스의 종류가 다양하지만
	// 지금은 Mesh와 Texture만 구현
	Mesh,
	Shader,
	Texture,
	Material,
	Animation,

	End
};

enum
{
	RESOURCE_TYPE_COUNT = static_cast<uint8>(ResourceType::End)
};

class ResourceBase : public enable_shared_from_this<ResourceBase>
{
public:
	ResourceBase(ResourceType type);
	virtual ~ResourceBase();

	ResourceType GetType() { return _type; }

	void SetName(const wstring& name) { _name = name; }
	const wstring& GetName() { return _name; }
	uint32 GetID() { return _id; }

protected:
	virtual void Load(const wstring& path) { }
	virtual void Save(const wstring& path) { }

protected:
	ResourceType _type = ResourceType::None;
	wstring _name;
	wstring _path;
	uint32 _id = 0;
};
```

```cpp
// Mesh는 정점(vertex)와 인덱스를 관리하게 된다.
class Mesh : public ResourceBase
{
    using Super = ResourceBase;

public:
    Mesh();
    virtual ~Mesh();

	void CreateQuad();
	void CreateCube();
	void CreateGrid(int32 sizeX, int32 sizeZ);
	void CreateSphere();

	shared_ptr<VertexBuffer> GetVertexBuffer() { return _vertexBuffer; }
	shared_ptr<IndexBuffer> GetIndexBuffer() { return _indexBuffer; }

private:
	void CreateBuffers();

private:
	// Mesh
	shared_ptr<Geometry<VertexTextureNormalData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer;
	shared_ptr<IndexBuffer> _indexBuffer;
};
```

```cpp
// Texture는 ShaderResourceView로 이미지를 관리한다.
class Texture : public ResourceBase
{
	using Super = ResourceBase;
public:
	Texture();
	~Texture();

	ComPtr<ID3D11ShaderResourceView> GetComPtr() { return _shaderResourveView; }

	virtual void Load(const wstring& path) override;

	Vec2 GetSize() { return _size; }

	const DirectX::ScratchImage& GetInfo() { return _img; }

private:
	ComPtr<ID3D11ShaderResourceView> _shaderResourveView;
	Vec2 _size = {0.f, 0.f};
	DirectX::ScratchImage _img = {};
};
```

* 좀 성격이 다르지만 IExecute는 Interface로
* 구현체의 Interface를 구성하고

```cpp
class IExecute
{
public:
	virtual void Init() abstract;
	virtual void Update() abstract;
	virtual void Render() abstract;
};
```

```cpp
struct GameDesc
{
	// 이런식으로 GameDesc에 받아와 사용하게 된다.
	shared_ptr<class IExecute> app = nullptr;
	wstring appName = L"GameCoding";
	
	// ...
};
```

---

## 1. Graphics

* Buffer와 Shader로 나뉜다
* Buffer는 알고있던 Constant, Geometry, Index, Vertex Buffer이며
* Shader가 구조가 많이 바꼈는데 이 부분은 모두 이해할 필요는 없고 아래와 같이 사용한다고 알자
	* 우선, Pass, Shader, Technique를 추가한다

```cpp
// shader클래스를 아래와 같이 선언하자
void TriangleDemo::Init()
{
	_shader = make_shared<Shader>(L"01. Triangle.fx");

	{
		_vertices.resize(3);

		_vertices[0].position = Vec3{-0.5f, 0.f, 0.f};
		_vertices[1].position = Vec3{0.f, 0.5f, 0.f};
		_vertices[2].position = Vec3{0.5f, 0.f, 0.f};
	}

	_buffer = make_shared<VertexBuffer>();
	_buffer->Create(_vertices);
}
```

```cpp
void TriangleDemo::Render()
{
	uint32 stride = _buffer->GetStride();
	uint32 offset = _buffer->GetOffset();

	DC->IASetVertexBuffers(0, 1, _buffer->GetComPtr().GetAddressOf(), &stride, &offset);

	// Draw할때 다음과 같이 tequenique와 pass를 지정한다
	_shader->Draw(1, 0, 3);
}
```

* 그럼 tequenique와 pass는 뭘까?

```cpp
// ...

// shader에서 아래와 같이 선언한 애들이다.
technique11 T0
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}

	pass P1
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS2()));
	}
};

technique11 T1
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS3()));
	}
};
```

* 일단 이런식으로 런타임에 Shader를 선택할 수 있다고 알아두자
* 이게 장점이 기존에 코드에서 Pixel, Vertex Shader를 선택하는 그 과정이 생략되었음.

---

## 2. Managers

* 자주사용되는, 전역으로 사용되는 애들을 모아두었다.
* Game, Graphics, Inputmanager, ResourceManager, TimeManager가 있으며, 크게 설명할건 없고 간단하게 하나만 보자

```cpp
// Graphics는 Device, Context, Swapchain등 Dx11기본적인 부분을 담당
class Graphics
{
	DECLARE_SINGLE(Graphics);

public:
	void Init(HWND hwnd);

	void RenderBegin();
	void RenderEnd();

	ComPtr<ID3D11Device> GetDevice() { return _device; }
	ComPtr<ID3D11DeviceContext> GetDeviceContext() { return _deviceContext; }

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void SetViewport();

private:
	HWND _hwnd = {};

	// Device & SwapChain
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
	ComPtr<IDXGISwapChain> _swapChain = nullptr;

	// RTV
	ComPtr<ID3D11RenderTargetView> _renderTargetView;

	// Misc
	D3D11_VIEWPORT _viewport = { 0 };
};
```

```cpp
void Game::Update()
{
	TIME->Update();
	INPUT->Update();

	GRAPHICS->RenderBegin();

	_desc.app->Update();
	_desc.app->Render();

	GRAPHICS->RenderEnd();
}
```

---

## 3. GameObject

```cpp
class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	~GameObject();

	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FixedUpdate();

	shared_ptr<Component> GetFixedComponent(ComponentType type);
	shared_ptr<Transform> GetTransform();
	shared_ptr<Camera> GetCamera();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	//shared_ptr<Animator> GetAnimator();

	shared_ptr<Transform> GetOrAddTransform();
	void AddComponent(shared_ptr<Component> component);

protected:
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
	vector<shared_ptr<MonoBehaviour>> _scripts;
};
```

* 나머지는 크게 뭐 설명할게 없어 생략 