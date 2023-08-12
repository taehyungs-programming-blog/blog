---
layout: default
title: "01. 구조 변경 + 테스트 방법 설명"
parent: "(DirectX11 3D Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/1)

---

## 하고자 하는 것은 이것이다

```cpp
// Client Main에 우리가 만든 Class를 담아 생성하면 렌더링이 일어나게 하고 싶다.

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	GameDesc desc;
	desc.appName = L"GameCoding";
	desc.hInstance = hInstance;
	desc.vsync = false;
	desc.hWnd = NULL;
	desc.width = 800;
	desc.height = 600;
	desc.clearColor = Color(0.5f, 0.5f, 0.5f, 0.5f);

    // MeshDemo가 우리가 만든 Class이다.
	desc.app = make_shared<MeshDemo>();

	GAME->Run(desc);

	return 0;
}
```

* 우리가 만들 Class의 구조는 아래와 같이 하고 싶다

```cpp
class TriangleDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;


	shared_ptr<Shader> _shader;
	vector<VertexData> _vertices;
	shared_ptr<VertexBuffer> _buffer;
};
```

* 나온김에 정리하자면 

### Shader class

* 간단히 말해서 shader를 등록하고 Draw를 도와주는 class이다.
* 가장 핵심적인 개념은 `ID3DX11Effect`(줄여서 Effect) 인데 주요 기능은
    * Techniques와 Passes를 이용해 다양한 렌더링을 정의가능
    * Constant Buffer를 사용하기 편하게 해줌

```cpp
// 가령 아래와 같이 shader에서 변수를 선언했다면
// 기존은 cpp코드까지 바꼈어야 하나 지금은 그렇게 하지 않아도 된다.

struct VertexInput
{
	float4 position : POSITION;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
};
```

### TriangleDemo 자체를 정리

```cpp
void TriangleDemo::Init()
{
    // 여기서 shader를 만들고
	_shader = make_shared<Shader>(L"01. Triangle.fx");

	{
        // vertex를 정의하고
		_vertices.resize(3);

		_vertices[0].position = Vec3{-0.5f, 0.f, 0.f};
		_vertices[1].position = Vec3{0.f, 0.5f, 0.f};
		_vertices[2].position = Vec3{0.5f, 0.f, 0.f};
	}

    // 여기서 vertex buffer를 만든다
	_buffer = make_shared<VertexBuffer>();
	_buffer->Create(_vertices);
}

void TriangleDemo::Update()
{

}

void TriangleDemo::Render()
{
    // stride는 vertex의 연속된 데이터 사이즈이다.
	uint32 stride = _buffer->GetStride();

    // offset은 vertex버퍼가 시작지점이 꼭 0이란 보장이 없기에 있는 개념
	uint32 offset = _buffer->GetOffset();

	DC->IASetVertexBuffers(0, 1, _buffer->GetComPtr().GetAddressOf(), &stride, &offset);

    // Draw(Technique, Pass, Vertext Count)
    // 이렇게 그려지게 된다.
	_shader->Draw(1, 0, 3);
}
```

---

* ??? 너무 어려운데 ???
* 예시로 보면 오히려 쉽다

## 네모 그리기

```cpp
class QuadDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;


	shared_ptr<Shader> _shader;

	shared_ptr<Geometry<VertexColorData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer;
	shared_ptr<IndexBuffer> _indexBuffer;
};
```

```cpp
void QuadDemo::Init()
{
    // 쉐이더 만들고
	_shader = make_shared<Shader>(L"02. Quad.fx");

    // geometry데이터 만들기, 만드는데만 쓰이기에 Init이후에 안씀
	_geometry = make_shared<Geometry<VertexColorData>>();
	GeometryHelper::CreateQuad(_geometry, Color(0.f, 1.f, 0.f, 1.f));

    // Vertex, Index buffer생성
	_vertexBuffer = make_shared<VertexBuffer>();
	_vertexBuffer->Create(_geometry->GetVertices());

	_indexBuffer = make_shared<IndexBuffer>();
	_indexBuffer->Create(_geometry->GetIndices());
}

void QuadDemo::Update()
{

}

void QuadDemo::Render()
{
	uint32 stride = _vertexBuffer->GetStride();
	uint32 offset = _vertexBuffer->GetOffset();

	DC->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

	//_shader->Draw(1, 0, 3);
	_shader->DrawIndexed(0, 1, _indexBuffer->GetCount(), 0, 0);
}
```

* 또 쉽네?
* 조금 더 보자

---

## Constant Buffer 써보기

```cpp
class ConstBufferDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;

	shared_ptr<Shader> _shader;

	shared_ptr<Geometry<VertexColorData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer;
	shared_ptr<IndexBuffer> _indexBuffer;

	Vec3 _translation = Vec3(0.f, 0.f, 0.f);

	Matrix _world = Matrix::Identity;
	Matrix _view = Matrix::Identity;
	Matrix _projection = Matrix::Identity;
};
```

```cpp
void ConstBufferDemo::Init()
{
	_shader = make_shared<Shader>(L"03. ConstBuffer.fx");

	_geometry = make_shared<Geometry<VertexColorData>>();
	GeometryHelper::CreateQuad(_geometry, Color(0.f, 1.f, 0.f, 1.f));

	_vertexBuffer = make_shared<VertexBuffer>();
	_vertexBuffer->Create(_geometry->GetVertices());

	_indexBuffer = make_shared<IndexBuffer>();
	_indexBuffer->Create(_geometry->GetIndices());
}

void ConstBufferDemo::Update()
{
	float dt = TIME->GetDeltaTime();

	if (INPUT->GetButton(KEY_TYPE::A))
	{
		_translation.x -= 3.f * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::D))
	{
		_translation.x += 3.f * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::W))
	{
		_translation.y += 3.f * dt;
	}
	else if (INPUT->GetButton(KEY_TYPE::S))
	{
		_translation.y -= 3.f * dt;
	}

	// SRT
	_world = Matrix::CreateTranslation(_translation);
}

void ConstBufferDemo::Render()
{
	_shader->GetMatrix("World")->SetMatrix((float*)&_world);
	_shader->GetMatrix("View")->SetMatrix((float*)&_view);
	_shader->GetMatrix("Projection")->SetMatrix((float*)&_projection);

	uint32 stride = _vertexBuffer->GetStride();
	uint32 offset = _vertexBuffer->GetOffset();

	DC->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

	_shader->DrawIndexed(0, 0, _indexBuffer->GetCount(), 0, 0);
}
```

```cpp
matrix World;
matrix View;
matrix Projection;

struct VertexInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.position = mul(input.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	output.color = input.color;

	return output;
}

float4 PS(VertexOutput input) : SV_TARGET
{
	return input.color;
}
```

---

## Camera 움직여 보기

```cpp
class CameraDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;

	shared_ptr<Shader> _shader;

	// Object
	shared_ptr<Geometry<VertexColorData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer;
	shared_ptr<IndexBuffer> _indexBuffer;
	Matrix _world = Matrix::Identity;

	// Camera
	shared_ptr<GameObject> _camera;
};
```

```cpp
void CameraDemo::Init()
{
	_shader = make_shared<Shader>(L"03. ConstBuffer.fx");

	// Object
	_geometry = make_shared<Geometry<VertexColorData>>();
	GeometryHelper::CreateQuad(_geometry, Color(0.f, 1.f, 0.f, 1.f));
	_vertexBuffer = make_shared<VertexBuffer>();
	_vertexBuffer->Create(_geometry->GetVertices());
	_indexBuffer = make_shared<IndexBuffer>();
	_indexBuffer->Create(_geometry->GetIndices());

	// Camera
	_camera = make_shared<GameObject>();
	_camera->GetOrAddTransform();
	_camera->AddComponent(make_shared<Camera>());
	_camera->AddComponent(make_shared<CameraScript>());

	_camera->GetTransform()->SetPosition(Vec3(0.f, 0.f, -2.f));
}

void CameraDemo::Update()
{
	_camera->Update();
}

void CameraDemo::Render()
{
	_shader->GetMatrix("World")->SetMatrix((float*)&_world);
	_shader->GetMatrix("View")->SetMatrix((float*)&Camera::S_MatView);
	_shader->GetMatrix("Projection")->SetMatrix((float*)&Camera::S_MatProjection);

	uint32 stride = _vertexBuffer->GetStride();
	uint32 offset = _vertexBuffer->GetOffset();

	DC->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

	_shader->DrawIndexed(0, 0, _indexBuffer->GetCount(), 0, 0);
}
```

---

## Texture 써보기

```cpp
class TextureDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;

	shared_ptr<Shader> _shader;

	// Object
	shared_ptr<Geometry<VertexTextureData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer;
	shared_ptr<IndexBuffer> _indexBuffer;
	Matrix _world = Matrix::Identity;

	// Camera
	shared_ptr<GameObject> _camera;

	shared_ptr<Texture> _texture;
};
```

```cpp
void TextureDemo::Init()
{
	_shader = make_shared<Shader>(L"04. Texture.fx");

	// Object
	_geometry = make_shared<Geometry<VertexTextureData>>();
	GeometryHelper::CreateQuad(_geometry);
	//GeometryHelper::CreateCube(_geometry);
	//GeometryHelper::CreateSphere(_geometry);
	//GeometryHelper::CreateGrid(_geometry, 256, 256);

	_vertexBuffer = make_shared<VertexBuffer>();
	_vertexBuffer->Create(_geometry->GetVertices());
	_indexBuffer = make_shared<IndexBuffer>();
	_indexBuffer->Create(_geometry->GetIndices());

	// Camera
	_camera = make_shared<GameObject>();
	_camera->GetOrAddTransform();
	_camera->AddComponent(make_shared<Camera>());
	_camera->AddComponent(make_shared<CameraScript>());
	_camera->GetTransform()->SetPosition(Vec3(0.f, 0.f, -2.f));

	_texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
}

void TextureDemo::Update()
{
	_camera->Update();
}

void TextureDemo::Render()
{
	_shader->GetMatrix("World")->SetMatrix((float*)&_world);
	_shader->GetMatrix("View")->SetMatrix((float*)&Camera::S_MatView);
	_shader->GetMatrix("Projection")->SetMatrix((float*)&Camera::S_MatProjection);
	_shader->GetSRV("Texture0")->SetResource(_texture->GetComPtr().Get());

	uint32 stride = _vertexBuffer->GetStride();
	uint32 offset = _vertexBuffer->GetOffset();

	DC->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

	_shader->DrawIndexed(0, 0, _indexBuffer->GetCount(), 0, 0);
}
```

```cpp
matrix World;
matrix View;
matrix Projection;
Texture2D Texture0;

struct VertexInput
{
	float4 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.position = mul(input.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	output.uv = input.uv;

	return output;
}

SamplerState Sampler0;

float4 PS(VertexOutput input) : SV_TARGET
{
	return Texture0.Sample(Sampler0, input.uv);
}
```

---

## Sampler 써보기

```cpp
class SamplerDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;

	shared_ptr<Shader> _shader;

	// Object
	shared_ptr<Geometry<VertexTextureData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer;
	shared_ptr<IndexBuffer> _indexBuffer;
	Matrix _world = Matrix::Identity;

	// Camera
	shared_ptr<GameObject> _camera;

	shared_ptr<Texture> _texture;
};
```

```cpp
void SamplerDemo::Init()
{
	_shader = make_shared<Shader>(L"05. Sampler.fx");

	// Object
	_geometry = make_shared<Geometry<VertexTextureData>>();
	//GeometryHelper::CreateQuad(_geometry);
	//GeometryHelper::CreateCube(_geometry);
	//GeometryHelper::CreateSphere(_geometry);
	GeometryHelper::CreateGrid(_geometry, 256, 256);

	_vertexBuffer = make_shared<VertexBuffer>();
	_vertexBuffer->Create(_geometry->GetVertices());
	_indexBuffer = make_shared<IndexBuffer>();
	_indexBuffer->Create(_geometry->GetIndices());

	// Camera
	_camera = make_shared<GameObject>();
	_camera->GetOrAddTransform();
	_camera->AddComponent(make_shared<Camera>());
	_camera->AddComponent(make_shared<CameraScript>());
	_camera->GetTransform()->SetPosition(Vec3(0.f, 0.f, -2.f));

	_texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
}

void SamplerDemo::Update()
{
	_camera->Update();
}

void SamplerDemo::Render()
{
	_shader->GetMatrix("World")->SetMatrix((float*)&_world);
	_shader->GetMatrix("View")->SetMatrix((float*)&Camera::S_MatView);
	_shader->GetMatrix("Projection")->SetMatrix((float*)&Camera::S_MatProjection);
	_shader->GetSRV("Texture0")->SetResource(_texture->GetComPtr().Get());

	enum ADDRESS_VALUE
	{
		ADDRESS_WRAP = 0,
		ADDRESS_MIRROR = 1,
		ADDRESS_CLAMP = 2,
		ADDRESS_BORDER = 3,
	};
	
	_shader->GetScalar("Address")->SetInt(ADDRESS_WRAP);

	uint32 stride = _vertexBuffer->GetStride();
	uint32 offset = _vertexBuffer->GetOffset();

	DC->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

	_shader->DrawIndexed(0, 0, _indexBuffer->GetCount(), 0, 0);
}
```

```cpp
matrix World;
matrix View;
matrix Projection;
Texture2D Texture0;
uint Address;

struct VertexInput
{
	float4 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.position = mul(input.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	output.uv = input.uv;

	return output;
}

// Filter = 확대/축소 일어났을 때 중간값을 처리하는 방식
// Address = UV가 1보다 컸을 때, 나머지 부분을 어떻게 처리
SamplerState Sampler0;

SamplerState SamplerAddressWrap
{
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState SamplerAddressMirror
{
	AddressU = Mirror;
	AddressV = Mirror;
};

SamplerState SamplerAddressClamp
{
	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerState SamplerAddressBorder
{
	AddressU = Border;
	AddressV = Border;
	BorderColor = float4(1, 0, 0, 1);
};

float4 PS(VertexOutput input) : SV_TARGET
{
	if (Address == 0)
		return Texture0.Sample(SamplerAddressWrap, input.uv);

	if (Address == 1)
		return Texture0.Sample(SamplerAddressMirror, input.uv);
	
	if (Address == 2)
		return Texture0.Sample(SamplerAddressWrap, input.uv);
	
	if (Address == 3)
		return Texture0.Sample(SamplerAddressBorder, input.uv);

	return Texture0.Sample(Sampler0, input.uv);
}
```

---

## HeightMap 써보기

* **HeightMap** - 지형지물 처럼 고도차이를 그리려한다.

```cpp
void HeightMapDemo::Init()
{
	_shader = make_shared<Shader>(L"06. Terrain.fx");

	// Texture
        // 지형 정보 이미지를 가져오고
	_heightMap = RESOURCES->Load<Texture>(L"Height", L"..\\Resources\\Textures\\Terrain\\height.png");
	_texture = RESOURCES->Load<Texture>(L"Grass", L"..\\Resources\\Textures\\Terrain\\grass.jpg");

	const int32 width = _heightMap->GetSize().x;
	const int32 height = _heightMap->GetSize().y;

	const DirectX::ScratchImage& info = _heightMap->GetInfo();
	uint8* pixelBuffer = info.GetPixels();

	// Object
	_geometry = make_shared<Geometry<VertexTextureData>>();
	GeometryHelper::CreateGrid(_geometry, width, height);

	// CPU
	{
		vector<VertexTextureData>& v = const_cast<vector<VertexTextureData>&>(_geometry->GetVertices());

		for (int32 z = 0; z < height; z++)
		{
			for (int32 x = 0; x < width; x++)
			{
                // 가져온 지형정보를 반영한다
				int32 idx = width * z + x;
				uint8 height = pixelBuffer[idx] / 255.f * 25.f;
				v[idx].position.y = height; // 높이 보정
			}
		}
	}

    // ...
```

---

## Normal 

* 물체의 표면에 수직인 벡터를 **Normal Vector**라 한다

```cpp
class NormalDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;

	shared_ptr<Shader> _shader;

	// Object
	shared_ptr<Geometry<VertexTextureNormalData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer;
	shared_ptr<IndexBuffer> _indexBuffer;
	Matrix _world = Matrix::Identity;

	// Camera
	shared_ptr<GameObject> _camera;
	shared_ptr<Texture> _texture;

    // Light를 y=-1로 고정하고
        // -1인 이유가 위에서 아래로 온다고 -1임.
	Vec3 _lightDir = Vec3(0.f, -1.f, 0.f);
};
```

```cpp
void NormalDemo::Render()
{
	_shader->GetMatrix("World")->SetMatrix((float*)&_world);
	_shader->GetMatrix("View")->SetMatrix((float*)&Camera::S_MatView);
	_shader->GetMatrix("Projection")->SetMatrix((float*)&Camera::S_MatProjection);
	_shader->GetSRV("Texture0")->SetResource(_texture->GetComPtr().Get());

    // Shader로 넘긴다
	_shader->GetVector("LightDir")->SetFloatVector((float*)&_lightDir);

	uint32 stride = _vertexBuffer->GetStride();
	uint32 offset = _vertexBuffer->GetOffset();

	DC->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

	_shader->DrawIndexed(0, 0, _indexBuffer->GetCount(), 0, 0);
}
```

```cpp
matrix World;
matrix View;
matrix Projection;
Texture2D Texture0;
float3 LightDir;

struct VertexInput
{
	float4 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.position = mul(input.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	output.uv = input.uv;
	output.normal = mul(input.normal, (float3x3)World);

	return output;
}

SamplerState Sampler0;

float4 PS(VertexOutput input) : SV_TARGET
{
	float3 normal = normalize(input.normal);
	float3 light = -LightDir;

	//return float4(1, 1, 1, 1) * dot(light, normal);
	return Texture0.Sample(Sampler0, input.uv) * dot(light, normal);
}
```

---

## Mesh

* 그려지는데 사용되는 Component를 Mesh로 묶어 사용해 보자

```cpp
void MeshDemo::Init()
{
	// Camera
	_camera = make_shared<GameObject>();
	_camera->GetOrAddTransform();
	_camera->AddComponent(make_shared<Camera>());
	_camera->AddComponent(make_shared<CameraScript>());
	
	// Object
	_obj = make_shared<GameObject>();
	_obj->GetOrAddTransform();

    // Component로 MeshRenderer를 붙이고
	_obj->AddComponent(make_shared<MeshRenderer>());

    // 그리기와 관련된 애들을 모두 MeshRenderer에 넣는다
	{
		auto shader = make_shared<Shader>(L"07. Normal.fx");
		_obj->GetMeshRenderer()->SetShader(shader);
	}
	{
		RESOURCES->Init();
		auto mesh = RESOURCES->Get<Mesh>(L"Sphere");
		_obj->GetMeshRenderer()->SetMesh(mesh);
	}
	{
		auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
		_obj->GetMeshRenderer()->SetTexture(texture);
	}
}
```

```cpp
class MeshRenderer : public Component
{
	using Super = Component;
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	virtual void Update() override;

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetTexture(shared_ptr<Texture> texture) { _texture = texture; }
	void SetShader(shared_ptr<Shader> shader) { _shader = shader; }

private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Texture> _texture;
	shared_ptr<Shader> _shader;
};
```

```cpp
void MeshRenderer::Update()
{
	if (_mesh == nullptr || _texture == nullptr || _shader == nullptr)
		return;

	auto world = GetTransform()->GetWorldMatrix();
	_shader->GetMatrix("World")->SetMatrix((float*)&world);
	
	_shader->GetMatrix("View")->SetMatrix((float*)&Camera::S_MatView);
	_shader->GetMatrix("Projection")->SetMatrix((float*)&Camera::S_MatProjection);
	_shader->GetSRV("Texture0")->SetResource(_texture->GetComPtr().Get());
	
	// TEMP
	Vec3 lightDir = {0.f, 0.f, 1.f};
	_shader->GetVector("LightDir")->SetFloatVector((float*)&lightDir);

	uint32 stride = _mesh->GetVertexBuffer()->GetStride();
	uint32 offset = _mesh->GetVertexBuffer()->GetOffset();

	DC->IASetVertexBuffers(0, 1, _mesh->GetVertexBuffer()->GetComPtr().GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_mesh->GetIndexBuffer()->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

	_shader->DrawIndexed(0, 0, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
}
```