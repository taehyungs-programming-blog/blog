---
layout: default
title: "13. UI"
parent: "(DirectX11 3D Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
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

## BillBoard

* object가 항상 내 방향으로 바라보게 만들고 싶다
* 이걸 어디쓰나?
	* 체력바 같은데 쓰면되겠지? 아니면 데미지, 아니먼 멀리 보이는 산, 나무 같은 부분

```cpp
// 방법1. 그냥 내 방향으로 틀자

void BillBoardTest::Update()
{
	auto go = GetGameObject();

	Vec3 up = Vec3(0, 1, 0);
	Vec3 cameraPos = CUR_SCENE->GetMainCamera()->GetTransform()->GetPosition();
	Vec3 myPos = GetTransform()->GetPosition();

	Vec3 forward = cameraPos - myPos;
	forward.Normalize();

	Matrix lookMatrix = Matrix::CreateWorld(myPos, forward, up);

	Vec3 S, T;
	Quaternion R;
	lookMatrix.Decompose(S, R, T);

	Vec3 rot = Transform::ToEulerAngles(R);

	GetTransform()->SetRotation(rot);
}
```

* 그런데 위 방식은 결국 렌더링 파이프라인을 모두 돌아야하기에 Instance로 만들고자 한다

```cpp
// Billboard용 클래스를 하나 만들자
	// 간단한 부분은 생략하고 생각할 만한 부분만 정리하면
Billboard::Billboard() : Super(ComponentType::BillBoard)
{
	int32 vertexCount = MAX_BILLBOARD_COUNT * 4;
	int32 indexCount = MAX_BILLBOARD_COUNT * 6;

	_vertices.resize(vertexCount);
	_vertexBuffer = make_shared<VertexBuffer>();
	_vertexBuffer->Create(_vertices, 0, true);

	_indices.resize(indexCount);

	// index를 넣는다
	for (int32 i = 0; i < MAX_BILLBOARD_COUNT; i++)
	{
		// 0, 1, 2
		_indices[i * 6 + 0] = i * 4 + 0;
		_indices[i * 6 + 1] = i * 4 + 1;
		_indices[i * 6 + 2] = i * 4 + 2;

		// 2, 1, 3
		_indices[i * 6 + 3] = i * 4 + 2;
		_indices[i * 6 + 4] = i * 4 + 1;
		_indices[i * 6 + 5] = i * 4 + 3;
	}

	_indexBuffer = make_shared<IndexBuffer>();
	_indexBuffer->Create(_indices);
}
```

```cpp
void Billboard::Update()
{
	if (_drawCount != _prevCount)
	{
		_prevCount = _drawCount;

		D3D11_MAPPED_SUBRESOURCE subResource;
		DC->Map(_vertexBuffer->GetComPtr().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			memcpy(subResource.pData, _vertices.data(), sizeof(VertexBillboard) * _vertices.size());
		}
		DC->Unmap(_vertexBuffer->GetComPtr().Get(), 0);
	}

	auto shader = _material->GetShader();

	// Transform
	auto world = GetTransform()->GetWorldMatrix();
	shader->PushTransformData(TransformDesc{world});

	// GlobalData
	shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);

	// Light
	_material->Update();

	// IA
	_vertexBuffer->PushData();
	_indexBuffer->PushData();

	shader->DrawIndexed(0, _pass, _drawCount * 6);
}
```

```cpp
struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float2 scale : SCALE;
};

struct V_OUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

V_OUT VS(VertexInput input)
{
    V_OUT output;

    float4 position = mul(input.position, W);

    float3 up = float3(0, 1, 0);
    float3 forward = position.xyz - CameraPosition();
    float3 right = normalize(cross(up, forward));

	// shader에서 임의로 billboard의 위치를 보정한다 생각하자
    position.xyz += (input.uv.x - 0.5f) * right * input.scale.x;
    position.xyz += (1.0f - input.uv.y - 0.5f) * up * input.scale.y;
    position.w = 1.0f;

    output.position = mul(mul(position, V), P);

    output.uv = input.uv;

    return output;
}

float4 PS(V_OUT input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.uv);

    if (diffuse.a < 0.3f)
        discard;

    return diffuse;
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
};
```

---

## Example) 눈을 만들어 보자

* 크게 다를건 없고 쉐이더만 보자면
	* 사실 이걸 파티클 시스템으로도 구현이 가능하다
	* 이건 billboard로 구현이 된다 정도로 이해하는게 맞을듯

```cpp
cbuffer SnowBuffer
{
    float4 Color;
    float3 Velocity;
    float DrawDistance;

    float3 Origin;
    float Turbulence;

    float3 Extent;
    float Time;
};

struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float2 scale : SCALE;
    float2 random : RANDOM;
};

struct V_OUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float alpha : ALPHA;
};

V_OUT VS(VertexInput input)
{
    V_OUT output;

    float3 displace = Velocity * Time * 100;

    input.position.y = Origin.y + Extent.y - (input.position.y - displace) % Extent.y;
    input.position.x += cos(Time - input.random.x) * Turbulence;
    input.position.z += cos(Time - input.random.y) * Turbulence;

    float4 position = mul(input.position, W);

    float3 up = float3(0, 1, 0);
    float3 forward = position.xyz - CameraPosition(); // BillBoard
    float3 right = normalize(cross(up, forward));

    position.xyz += (input.uv.x - 0.5f) * right * input.scale.x;
    position.xyz += (1.0f - input.uv.y - 0.5f) * up * input.scale.y;
    position.w = 1.0f;

    output.position = mul(mul(position, V), P);
    output.uv = input.uv;

    output.alpha = 1.0f;

    // Alpha Blending
    float4 view = mul(position, V);
    output.alpha = saturate(1 - view.z / DrawDistance) * 0.8f;

    return output;
}

float4 PS(V_OUT input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.uv);

    diffuse.rgb = Color.rgb * input.alpha * 2.0f;
    diffuse.a = diffuse.a * input.alpha * 1.5f;

    return diffuse;
}

technique11 T0
{
    PASS_BS_VP(P0, AlphaBlend, VS, PS)
};
```